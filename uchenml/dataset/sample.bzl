""" Definition of the sampling rule """

load("//uchenml:providers.bzl", "DatasetInfo")

def sample_impl(ctx):
    """
    This function implements the sample rule.

    Args:
      ctx: The context object containing attributes and actions.

    Returns:
      The DefaultInfo object containing the output manifest.
    """
    output_dir = ctx.actions.declare_directory(ctx.attr.name)
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
    args.add("--output", output_dir.path)
    args.add("--seed", ctx.attr.seed)
    args.add("--stderrthreshold=0")
    args.add(directory.path)
    ctx.actions.run(
        inputs = [directory],
        outputs = [output_dir],
        executable = ctx.executable._sampler_bin,
        arguments = [args],
        progress_message = "Sampling " + ctx.attr.name,
    )
    return [DefaultInfo(
        files = depset([output_dir]),
    ), DatasetInfo(
        data = [output_dir],
        max_samples = ctx.attr.samples,
    )]
