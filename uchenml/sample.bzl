""" Definition of the sampling rule """

def sample_impl(ctx):
    """
    This function implements the sample rule.

    Args:
      ctx: The context object containing attributes and actions.

    Returns:
      The DefaultInfo object containing the output manifest.
    """
    manifest = ctx.actions.declare_file(ctx.attr.name + ".manifest")
    directory = ctx.files.src[0]
    args = ctx.actions.args()
    args.add_joined("--included", ctx.attr.include, join_with = ",", uniquify = True, expand_directories = False)
    args.add_joined("--excluded", ctx.attr.exclude, join_with = ",", uniquify = True, expand_directories = False)
    args.add_joined("--ext", ctx.attr.extensions, join_with = ",", uniquify = True)
    args.add("--min_size", ctx.attr.min_size)
    if ctx.attr.max_size > 0:
      args.add("--max_size", ctx.attr.max_size)
    if ctx.attr.samples > 0:
      args.add("--samples", ctx.attr.samples)
    args.add("--output", manifest)
    args.add("--seed", ctx.attr.seed)
    args.add(directory.path)
    ctx.actions.run(
        inputs = [directory],
        outputs = [manifest],
        executable = ctx.executable._sampler_bin,
        arguments = [args],
        progress_message = "Sampling " + ctx.attr.name,
    )
    return DefaultInfo(
        files = depset([manifest]),
        runfiles = ctx.runfiles(transitive_files = depset([directory])),
    )
