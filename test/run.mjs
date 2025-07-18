import path from 'path'
import fs from 'fs'
import { spawn } from 'node:child_process'

import chalk from 'chalk'


function runSpec (runFile, args, inputMaybe, doneOk, doneErr, doneRunaway) {
    let timeoutHandle = null
    let killRequested = false

    let out = ''
    let err = ''

    const child = spawn(
        path.isAbsolute(runFile) ? runFile : path.join(import.meta.dirname, runFile),
        args,
    )

    if (inputMaybe != null) {
        inputMaybe.pipe(child.stdin)
    }

    child.stdout.on('data', (data) => {
        if (out.length < 1024) {
            out += data
        }
    })

    child.stderr.on('data', (data) => {
        if (err.length < 1024) {
            err += data
        }
    })

    child.on('close', (code, signal) => {
        if (killRequested) {
            doneRunaway(out, err)
            return
        }

        clearTimeout(timeoutHandle)

        if (code === 0) {
            doneOk(out, err)
        } else {
            doneErr(out, err, code, signal)
        }
    })

    timeoutHandle = setTimeout(() => {
        killRequested = true
        child.kill('SIGINT')
    }, 500)
}

function collect (name, sub, makeArgs, entries) {
    entries.push(name)

    const allFileEntries = fs.readdirSync(path.join(import.meta.dirname, 'spec', sub), { withFileTypes: true })

    for (const entry of allFileEntries) {
        if (!entry.isFile()) {
            continue
        }

        const { base, name, ext } = path.parse(entry.name)

        if (ext !== '.bf') {
            continue
        }

        const expFile = path.join(entry.parentPath, `${name}.exp`)

        if (!fs.existsSync(expFile)) {
            continue
        }

        const inputFile = path.join(entry.parentPath, `${name}.in`)

        const sourceFile = path.join(entry.parentPath, base)

        entries.push({
            sub,
            name,
            sourceFile,
            expFile,
            inputFile: fs.existsSync(inputFile) ? inputFile : null,
            args: makeArgs(sourceFile),
        })
    }
}

function stringTrimEquals (aRaw, bRaw) {
    if (aRaw === bRaw) {
        return { match: true, grade: 'perfect' }
    }

    const aTrim = aRaw.trimEnd()
    const bTrim = bRaw.trimEnd()

    if (aTrim === bTrim) {
        return { match: true, grade: 'trim-end' }
    }

    const aLines = aTrim.split('\n')
    const bLines = bTrim.split('\n')

    for (let i = 0; i < aLines.length; i++) {
        const aLine = aLines[i]
        const bLine = bLines[i]

        if (aLine.trimEnd() !== bLine.trimEnd()) {
            return { match: false, lineIndex: i }
        }
    }

    return { match: true, grade: 'trim-lines' }
}

function consume (entries) {
    let index = 0
    let anyError = false

    function step () {
        if (index >= entries.length) {
            return
        }

        const entry = entries[index]
        index++

        if (typeof entry === 'string') {
            if (anyError) {
                process.stdout.write(`some tests failed; skipping remaining tests`)
                return
            }

            if (index > 1) {
                process.stdout.write('\n')
            }

            process.stdout.write(chalk.bold(`${entry.padEnd(20, ' ')}\n`))
            setImmediate(step)
            return
        }

        const inputMaybe = entry.inputFile == null ? null : fs.createReadStream(entry.inputFile)

        process.stdout.write(`${entry.name.padEnd(20, ' ')}    `)

        runSpec(
            '../cmake-build-debug/b86',
            entry.args,
            inputMaybe,
            (outRaw, err) => {
                const expRaw = fs.readFileSync(entry.expFile, { encoding: 'utf-8' })

                const equalsResult = stringTrimEquals(outRaw, expRaw)

                if (equalsResult.match) {
                    process.stdout.write(chalk.green(`ok`))
                    process.stdout.write(chalk.italic.dim(`    ${equalsResult.grade}\n`))
                } else {
                    anyError = true
                    process.stdout.write(chalk.red(`fail\n`))

                    process.stdout.write(`expected '${expRaw}'\n`)
                    process.stdout.write(`but got  '${outRaw}'\n`)
                    process.stdout.write(`mismatch on line ${equalsResult.lineIndex + 1}\n`)
                    process.stdout.write(`\n`)
                }

                setImmediate(step)
            },
            (outRaw, err, code, signal) => {
                anyError = true
                const expRaw = fs.readFileSync(entry.expFile, { encoding: 'utf-8' })

                process.stdout.write(chalk.red(`fail\n`))

                process.stdout.write(`expected '${expRaw}'\n`)
                process.stdout.write(`got      '${outRaw}'\n`)
                process.stdout.write(`err      '${err}'\n`)
                process.stdout.write(`code     ${code}\n`)
                process.stdout.write(`signal   ${signal}\n`)

                setImmediate(step)
            },
            (out, err) => {
                anyError = true
                process.stdout.write(`runaway\n`)

                setImmediate(step)
            },
        )
    }

    step()
}

function runAll () {
    const entries = []

    collect('read-playfield', 'read-playfield', (sourceFile) => ['read-playfield', sourceFile], entries)

    collect('find-pathlet', 'find-pathlet', (sourceFile) => ['find-pathlet', sourceFile], entries)

    collect('find-graph', 'find-graph', (sourceFile) => ['find-graph', sourceFile], entries)

    collect('run-line (never-opt)', 'run-line', (sourceFile) => ['run-line', sourceFile], entries)

    collect('run (never-opt)', 'run', (sourceFile) => ['--never-opt', sourceFile], entries)

    collect('run (more specs, never-opt)', 'run-more', (sourceFile) => ['--never-opt', sourceFile], entries)

    collect('run (run-line specs)', 'run-line', (sourceFile) => [sourceFile], entries)

    collect('run', 'run', (sourceFile) => [sourceFile], entries)

    collect('run (more specs)', 'run-more', (sourceFile) => [sourceFile], entries)

    consume(entries)
}

runAll()