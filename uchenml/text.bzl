""" Rules for text manipulation. """

load("providers.bzl", "DatasetInfo")
load("utils.bzl", "build_outputs")

def _tool_invocation(ctx, inputs, outputs, label):
    args = ctx.actions.args()
    args.add_joined("--inputs", inputs, join_with = ",")
    args.add_joined("--outputs", outputs, join_with = ",")
    args.add("--seed", abs(ctx.attr.seed ^ hash(label)))
    args.add("--min_length", ctx.attr.min_length)
    args.add("--max_length", ctx.attr.max_length)
    args.add("--stderrthreshold", 0)
    ctx.actions.run(
        inputs = inputs,
        outputs = outputs,
        executable = ctx.file._extractor_tool,
        arguments = [args],
        progress_message = label,
    )

def _extract_text_impl(ctx):
    srcs = ctx.files.srcs
    outputs = []
    outputs = [
        ctx.actions.declare_file(f)
        for f in build_outputs(
            ctx.attr.name,
            len(srcs),
            ctx.attr.max_samples_per_dir,
        )
    ]
    per_invocation = ctx.attr.files_per_tool_invocation
    invocations = (len(srcs) + per_invocation - 1) // per_invocation
    for i in range(invocations):
        start = i * per_invocation
        _tool_invocation(
            ctx,
            srcs[start:start + per_invocation],
            outputs[start:start + per_invocation],
            "Extracting text, batch %d of %d" % (
                i + 1,
                invocations,
            ),
        )
    return [DefaultInfo(files = depset(outputs))]

extract_text = rule(
    implementation = _extract_text_impl,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            providers = [DatasetInfo, DefaultInfo],
        ),
        "min_length": attr.int(
            default = 0,
            doc = "Minimum length of the extracted text.",
        ),
        "max_length": attr.int(
            default = 0,
            doc = "Maximum length of the extracted text.",
        ),
        "seed": attr.int(
            default = 42,
            doc = "Seed for the random number generator.",
        ),
        "files_per_tool_invocation": attr.int(
            default = 8,
            doc = "Number of files to process in each invocation of the extractor tool.",
        ),
        "max_samples_per_dir": attr.int(
            default = 1000,
            doc = "Maximum number of samples per directory",
        ),
        "_extractor_tool": attr.label(
            allow_single_file = True,
            default = Label("//src/text:extractor"),
            executable = True,
            cfg = "exec",
        ),
    },
    doc = "Extracts text from a file.",
)
