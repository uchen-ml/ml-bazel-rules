""" Definition of the sampling rule """

load("//uchenml:providers.bzl", "DatasetInfo")

def _zfill(value, width):
    for _ in range(width):
        if len(value) >= width:
            return value
        value = "0" + str(value)
    return value

def sample_impl(ctx):
    """
    This function implements the sample rule.

    Args:
      ctx: The context object containing attributes and actions.

    Returns:
      The DefaultInfo object containing the output manifest.
    """
    batch_size = ctx.attr.batch_size
    batches = (ctx.attr.samples + batch_size - 1) // batch_size
    manifests = []
    longest = len(str(batches))
    for i in range(1, batches + 1):
        manifests.append(ctx.actions.declare_file(ctx.attr.name + "_%s.manifest" % _zfill(str(i), longest)))
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
    args.add_joined("--outputs", manifests, join_with = ",", uniquify = True)
    args.add("--seed", ctx.attr.seed)
    args.add("--stderrthreshold=0")
    args.add(directory.path)
    ctx.actions.run(
        inputs = [directory],
        outputs = manifests,
        executable = ctx.executable._sampler_bin,
        arguments = [args],
        progress_message = "Sampling " + ctx.attr.name,
    )
    return [DefaultInfo(
        files = depset(manifests),
    ), DatasetInfo(
        manifests = manifests,
        directory = directory,
    )]
