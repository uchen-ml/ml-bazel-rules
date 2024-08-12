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
            fail("The dataset must have a directory")
        manifests.append(target[DatasetInfo].manifest)
    output = ctx.actions.declare_file(ctx.attr.name + ".bow")
    args = ctx.actions.args()
    args.add_joined("--inputlists", manifests, join_with = ",", uniquify = True)
    args.add("--output", output)
    ctx.actions.run(
        inputs = ctx.files.srcs,
        outputs = [output],
        executable = ctx.file._bow_tool,
        arguments = [args],
        progress_message = "Creating bag of words %s" % ctx.label.name,
    )
    return DefaultInfo(files = depset([output]))
