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
    samples = ctx.attr.samples
    if samples < 1:
        fail("No samples were requested: %d" % samples)
    samples_per_dir = ctx.attr.max_samples_per_dir
    dirs = (samples + samples_per_dir - 1) // samples_per_dir
    name = ctx.attr.name
    name_gen = lambda i: "%s/%d.sample" % (name, i + 1)
    if dirs > 1:
        name_gen = lambda i: "%s/%d/%d.sample" % (
            name,
            (i % dirs) + 1,
            (i // dirs) + 1,
        )
    outputs = sorted([ctx.actions.declare_file(name_gen(i)) for i in range(samples)])
    dirname = outputs[0].dirname
    if dirs > 1:
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
    args.add("--per_dir", samples_per_dir)
    args.add("--stderrthreshold", "0")
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
