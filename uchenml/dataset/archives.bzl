""" Rules for downloading files. """

def http_download_impl(ctx):
    """
    This function implements the http_download rule.

    Args:
      ctx: The context object containing attributes and actions.

    Returns:
      The DefaultInfo object containing the output file.
    """
    extension = ctx.attr.url.rsplit(".", 1)[-1]
    if ctx.attr.url.endswith(".tar.gz"):
        extension = "tar.gz"
    file_name = ctx.attr.name + "." + extension
    output = ctx.actions.declare_file(file_name)
    temp = ctx.actions.declare_file(file_name + ".download")
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

def unpack_impl(ctx):
    """
    This function implements the unpack rule.

    Args:
      ctx: The context object containing attributes and actions.

    Returns:
      The DefaultInfo object containing the output directory.
    """
    output = ctx.actions.declare_directory(ctx.attr.name)
    inp = ctx.file.input
    args = ctx.actions.args()
    args.add("-xzf", inp)
    if (ctx.attr.strip_prefix_segments > 0):
        args.add(ctx.attr.strip_prefix_segments, format = "--strip-components=%s")
    args.add_all(["-C", output.path])
    ctx.actions.run(
        executable = "tar",
        inputs = [inp],
        outputs = [output],
        arguments = [args],
        progress_message = "Unpacking " + inp.path,
    )
    return DefaultInfo(files = depset([output]))
