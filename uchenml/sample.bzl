""" Definition of the sampling rule """

def sample_impl(ctx):
    """
    This function implements the sample rule.

    Args:
      ctx: The context object containing attributes and actions.

    Returns:
      The DefaultInfo object containing the output manifest.
    """
    filelist = ctx.actions.declare_file(ctx.attr.name + ".filelist")
    files = ctx.actions.args()
    files.add_all(ctx.attr.dataset.files)
    inps = depset([filelist], transitive = [ctx.attr.dataset.files])
    ctx.actions.write(filelist, files)
    output = ctx.actions.declare_file(ctx.attr.name + ".manifest")
    args = ctx.actions.args()
    base = ctx.files.dataset[0].path
    args.add("--base", base)
    args.add("--filelist", filelist)
    args.add("--stderrthreshold=0")
    args.add_joined("--include", [base + "/" + d for d in ctx.attr.include_dirs], join_with = ",")
    args.add_joined("--extension", [ext if ext.startswith(".") else "." + ext for ext in ctx.attr.extensions], join_with = ",")
    ctx.actions.run(
        inputs = inps,
        outputs = [output],
        executable = ctx.executable._sampler_bin,
        arguments = [args],
        progress_message = "Sampling " + ctx.attr.name,
    )
    return DefaultInfo(files = depset([output]), runfiles = ctx.runfiles(transitive_files = inps))
