""" Definition of the sampling rule """

load("//uchenml:providers.bzl", "DatasetInfo")
load("//uchenml:utils.bzl", "samples_dir_count")

def sample_impl(ctx):
    """
    This function implements the sample rule.

    Args:
      ctx: The context object containing attributes and actions.

    Returns:
      The DefaultInfo object containing the output manifest.
    """
    samples = ctx.attr.samples
    if samples < 1:
        fail("No samples were requested: %d" %samples)
    dirs = samples_dir_count(samples)
    name = ctx.attr.name
    outputs = []
    for i in range(samples):
        outputs.append(ctx.actions.declare_file("%s/%d/%d.sample" % (name, (i % dirs) + 1, (i // dirs) + 1)))
    dirname = outputs[0].dirname
    dirname = dirname[:dirname.rfind("/")]
    directory = ctx.files.src[0]
    args = ctx.actions.args()
    args.add_joined("--included", ctx.attr.include, join_with = ",", uniquify = True, expand_directories = False)
    args.add_joined("--excluded", ctx.attr.exclude, join_with = ",", uniquify = True, expand_directories = False)
    args.add_joined("--ext", ctx.attr.extensions, join_with = ",", uniquify = True)
    args.add("--min_size", ctx.attr.min_size)
    if ctx.attr.max_size > 0:
        args.add("--max_size", ctx.attr.max_size)
    args.add("--samples", ctx.attr.samples)
    args.add("--output", dirname)
    args.add("--seed", ctx.attr.seed)
    args.add("--dirs", dirs)
    args.add(directory.path)
    ctx.actions.run(
        inputs = [directory],
        outputs = outputs,
        executable = ctx.executable._sampler_bin,
        arguments = [args],
        progress_message = "Sampling " + ctx.attr.name,
    )
    return [DefaultInfo(
        files = depset(outputs),
    ), DatasetInfo(
        data = depset(outputs),
    )]
