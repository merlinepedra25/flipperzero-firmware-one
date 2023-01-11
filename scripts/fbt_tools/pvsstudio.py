from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Script import Delete, Mkdir
import multiprocessing


def emit_pvsreport(target, source, env):
    target_dir = env["REPORT_DIR"]
    if env["PLATFORM"] == "win32":
        # Report generator on Windows emits to a subfolder of given output folder
        target_dir = target_dir.Dir("fullhtml")
    return [target_dir.File("index.html")], source


def generate(env):
    env.SetDefault(
        PVSNCORES=multiprocessing.cpu_count(),
        PVSOPTIONS=[
            "@.pvsoptions",
            "-j${PVSNCORES}",
            # "--incremental", # kinda broken on PVS side
        ],
        PVSCONVOPTIONS=[
            "-a",
            "GA:1,2,3",
            "-t",
            "fullhtml",
            "--indicate-warnings",
        ],
    )

    if env["PLATFORM"] == "win32":
        env.SetDefault(
            PVSCHECKBIN="CompilerCommandsAnalyzer.exe",
            PVSCONVBIN="PlogConverter.exe",
        )
    else:
        env.SetDefault(
            PVSCHECKBIN="pvs-studio-analyzer",
            PVSCONVBIN="plog-converter",
        )

    if not env["VERBOSE"]:
        env.SetDefault(
            PVSCHECKCOMSTR="\tPVS\t${TARGET}",
            PVSCONVCOMSTR="\tPVSREP\t${TARGET}",
        )

    env.Append(
        BUILDERS={
            "PVSCheck": Builder(
                action=Action(
                    '${PVSCHECKBIN} analyze ${PVSOPTIONS} -f "${SOURCE}" -o "${TARGET}"',
                    "${PVSCHECKCOMSTR}",
                ),
                suffix=".log",
                src_suffix=".json",
            ),
            "PVSReport": Builder(
                action=[
                    Delete("${TARGET.dir}"),
                    # PlogConverter.exe and plog-converter have different behavior
                    Mkdir("${TARGET.dir}") if env["PLATFORM"] == "win32" else None,
                    Action(
                        '${PVSCONVBIN} ${PVSCONVOPTIONS} "${SOURCE}" -o "${REPORT_DIR}"',
                        "${PVSCONVCOMSTR}",
                    ),
                ],
                emitter=emit_pvsreport,
                src_suffix=".log",
            ),
        }
    )


def exists(env):
    return True