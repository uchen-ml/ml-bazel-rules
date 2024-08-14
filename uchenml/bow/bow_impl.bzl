""" Bag of words rule implementation """

load("//uchenml:providers.bzl", "DatasetInfo")

def bag_of_words_impl(ctx):
    """
    Bag of words rule implementation

    Args:
      ctx: Rule context

    Returns:
      DefaultInfo
    """
    manifests = []
    for target in ctx.attr.srcs:
        if not DatasetInfo in target:
            fail("Target %s is not a dataset" % target.label)
        manifests = manifests + target[DatasetInfo].manifests
    intermediate = []
    i = 1
    for manifest in manifests:
        intermediate.append(ctx.actions.declare_file(ctx.attr.name + "_%s.bow" % i))
        i += 1
        args = ctx.actions.args()
        args.add_joined("--inputs", [manifest], join_with = ",", uniquify = True)
        args.add("--output", intermediate[-1])
        # args.add("--stderrthreshold=0")
        ctx.actions.run(
            inputs = ctx.files.srcs,
            outputs = [intermediate[-1]],
            executable = ctx.file._bow_tool,
            arguments = [args],
            progress_message = "Creating bag of words %s" % ctx.label.name,
        )
    output = ctx.actions.declare_file(ctx.attr.name + ".bow")
    args = ctx.actions.args()
    args.add_joined("--combine", intermediate, join_with = ",", uniquify = True)
    args.add("--output", output)
    # args.add("--stderrthreshold=0")
    ctx.actions.run(
        inputs = intermediate,
        outputs = [output],
        executable = ctx.file._bow_tool,
        arguments = [args],
        progress_message = "Combining bag of words %s" % ctx.label.name,
    )
    return DefaultInfo(files = depset([output]))
