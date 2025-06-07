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

function collect (sub, makeArgs, entries) {
    entries.push(sub)

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

            process.stdout.write(chalk.bold(`${entry.padEnd(16, ' ')}\n`))
            setImmediate(step)
            return
        }

        const inputMaybe = entry.inputFile == null ? null : fs.createReadStream(entry.inputFile)

        process.stdout.write(`${entry.name.padEnd(16, ' ')}    `)

        runSpec(
            '../cmake-build-debug/b86',
            entry.args,
            inputMaybe,
            (outRaw, err) => {
                const expRaw = fs.readFileSync(entry.expFile, { encoding: 'utf-8' })

                const outTrim = outRaw.trimEnd()
                const expTrim = expRaw.trimEnd()

                if (outTrim === expTrim) {
                    process.stdout.write(chalk.green(`ok\n`))
                } else {
                    anyError = true
                    process.stdout.write(chalk.red(`fail\n`))

                    process.stdout.write(`expected '${expTrim}'\n`)
                    process.stdout.write(`but got  '${outTrim}'\n`)
                    process.stdout.write(`\n`)
                }

                setImmediate(step)
            },
            (outRaw, err, code, signal) => {
                anyError = true
                const expRaw = fs.readFileSync(entry.expFile, { encoding: 'utf-8' })

                const outTrim = outRaw.trimEnd()
                const expTrim = expRaw.trimEnd()

                process.stdout.write(chalk.red(`fail\n`))

                process.stdout.write(`expected '${expTrim}'\n`)
                process.stdout.write(`got      '${outTrim}'\n`)
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

    collect('read-playfield', (sourceFile) => ['read-playfield', sourceFile], entries)

    collect('find-pathlet', (sourceFile) => ['find-pathlet', sourceFile], entries)

    collect('find-graph', (sourceFile) => ['find-graph', sourceFile], entries)

    collect('run-line', (sourceFile) => ['run-line', sourceFile], entries)

    collect('run', (sourceFile) => [sourceFile], entries)

    collect('run-more', (sourceFile) => [sourceFile], entries)

    consume(entries)
}

runAll()