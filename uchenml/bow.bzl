"""Bag-of-words construction rules"""

load("utils.bzl", "build_outputs")

def _generate_intermediate(ctx, inputs, output, label):
    args = ctx.actions.args()
    args.add_joined("--inputs", inputs, join_with = ",")
    args.add("--output", output)
    # args.add("--stderrthreshold", 0)
    ctx.actions.run(
        inputs = inputs,
        outputs = [output],
        executable = ctx.file._bow_tool,
        arguments = [args],
        progress_message = label,
    )

def _bag_of_words_impl(ctx):
    inputs = ctx.files.srcs
    per_inv = ctx.attr.files_per_invocation
    batches = (len(inputs) + per_inv - 1) // per_inv
    intermediate = [
        ctx.actions.declare_file(f)
        for f in build_outputs(
            ctx.label.name,
            batches,
            ctx.attr.max_files_per_dir,
            "intermediate.bow"
        )
    ]
    for i in range(batches):
        start = i * per_inv
        inps = inputs[start:start + per_inv]
        _generate_intermediate(
            ctx,
            inps,
            intermediate[i],
            "intermediate result for %s, batch %d of %d" % (
                ctx.label.name,
                i + 1,
                batches,
            ),
        )
    output = ctx.actions.declare_file(ctx.attr.name + ".bow")
    args = ctx.actions.args()
    args.add_joined("--combine", intermediate, join_with = ",", uniquify = True)
    args.add("--output", output)
    args.add("--stderrthreshold=0")
    ctx.actions.run(
        inputs = intermediate,
        outputs = [output],
        executable = ctx.file._bow_tool,
        arguments = [args],
        progress_message = "Combining bag of words %s" % ctx.label.name,
    )
    return DefaultInfo(files = depset([output]))

bag_of_words = rule(
    implementation = _bag_of_words_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True, mandatory = True),
        "size": attr.int(mandatory = True),
        "files_per_invocation": attr.int(default = 32),
        "max_files_per_dir": attr.int(default = 1000),
        "_bow_tool": attr.label(
            default = Label("//src/bow"),
            cfg = "exec",
            executable = True,
            allow_single_file = True,
        ),
    },
)
