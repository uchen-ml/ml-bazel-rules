""" Bag of words rule implementation """

def bag_of_words_impl(ctx):
    """
    Bag of words rule implementation

    Args:
      ctx: Rule context

    Returns:
      DefaultInfo
    """
    output = ctx.actions.declare_file(ctx.attr.name + ".bow")
    args = ctx.actions.args()
    args.add_joined("--inputlists", ctx.files.srcs, join_with = ",", uniquify = True)
    args.add("--output", output)
    args.add("--stderrthreshold=0")
    ctx.actions.run(
        inputs = ctx.files.srcs,
        outputs = [output],
        executable = ctx.file._bow_tool,
        arguments = [args],
    )
    return DefaultInfo(files = depset([output]))
