"""Common definitions for UchenML rules for training data management."""

# TODO: Git, unpack, sample

def _get_name(url):
    return url.rsplit("/", 1)[-1]

def _curl_impl(ctx):
    output = ctx.outputs.output
    if not output:
        output = ctx.actions.declare_file(_get_name(ctx.attr.url))
    temp = ctx.actions.declare_file(output.path + ".download")
    ctx.actions.run(
        executable = "curl",
        inputs = [],
        outputs = [temp],
        arguments = [
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
