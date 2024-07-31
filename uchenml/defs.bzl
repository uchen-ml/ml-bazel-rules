"""Common definitions for UchenML rules for training data management."""

# TODO: Git, unpack, sample

def _curl_impl(ctx):
    extension = ctx.attr.url.rsplit(".", 1)[-1]
    if ctx.attr.url.endswith(".tar.gz"):
        extension = "tar.gz"
    output = ctx.actions.declare_file(ctx.attr.name + "." + extension)
    temp = ctx.actions.declare_file(output.path + ".download")
    ctx.actions.run(
        executable = "curl",
        inputs = [],
        outputs = [temp],
        arguments = [
            "--fail",
            "-sSL",
            ctx.attr.url,
            "-o",
            temp.path,
        ],
        progress_message = "Downloading " + ctx.attr.url,
    )
    if ctx.attr.md5:
        ctx.actions.run(
            inputs = [temp],
            outputs = [output],
            executable = ctx.executable._check_and_copy,
            arguments = [
                temp.path,
                output.path,
                ctx.attr.md5,
            ],
            progress_message = "Checking MD5 of " + ctx.attr.url,
        )
    else:
        ctx.actions.run_shell(
            inputs = [temp],
            outputs = [output],
            command = "cp %s %s" % (temp.path, output.path),
            progress_message = "Copying " + ctx.attr.url,
        )
    return DefaultInfo(files = depset([output]))

def _unpack_impl(ctx):
    output = ctx.actions.declare_directory(ctx.attr.name)
    inp = ctx.attr.input.files.to_list()[0]
    input_path = inp.path
    if input_path.endswith(".tar.gz"):
        ctx.actions.run(
            executable = "tar",
            inputs = [inp],
            outputs = [output],
            arguments = [
                "-xzf",
                input_path,
                "-C",
                output.path,
            ],
            progress_message = "Unpacking " + input_path,
        )
    elif input_path.endswith(".zip"):
        ctx.actions.run(
            executable = "unzip",
            inputs = [inp],
            outputs = [output],
            arguments = [
                "-q",
                input_path,
                "-d",
                output.path,
            ],
            progress_message = "Unpacking " + input_path,
        )
    else:
        fail("Unsupported archive format: " + input_path)
    return DefaultInfo(files = depset([output]))

http_download = rule(
    implementation = _curl_impl,
    attrs = {
        "md5": attr.string(mandatory = False),
        "output": attr.output(mandatory = False),
        "url": attr.string(mandatory = True),
        "_check_and_copy": attr.label(
            cfg = "exec",
            default = Label("//scripts:check_and_copy"),
            executable = True,
        ),
    },
)

unpack = rule(
    implementation = _unpack_impl,
    attrs = {
        "input": attr.label(
            allow_single_file = [".tar.gz", ".zip"],
            mandatory = True,
        ),
    },
)
