""" Rules for text manipulation. """

load("//uchenml:utils.bzl", "samples_dir_count")
load("providers.bzl", "DatasetInfo")

def _file_name(name, samples_per_dir, n):
    return "%s/%d/%d.sample" % (name, n // samples_per_dir + 1, n % samples_per_dir + 1)

def _extract_text_impl(ctx):
    srcs = ctx.files.srcs
    dirs = samples_dir_count(len(srcs))
    samples_per_dir = len(srcs) // dirs
    outputs = []
    files_per_call = ctx.attr.files_per_call
    for d in range(dirs):
        for s in range(samples_per_dir // files_per_call + 1):
            start = d * samples_per_dir + s * files_per_call
            action_inputs = srcs[start:start + files_per_call]
            action_outputs = [
                ctx.actions.declare_file(_file_name(ctx.attr.name, samples_per_dir, i))
                for i in range(start, start + len(action_inputs))
            ]
            print(action_inputs)
            print(action_outputs)
            args = ctx.actions.args()
            args.add_joined("--inputs", action_inputs, join_with = ",")
            args.add_joined("--outputs", action_outputs, join_with = ",")
            args.add("--seed", ctx.attr.seed + start)
            args.add("--min_length", ctx.attr.min_length)
            args.add("--max_length", ctx.attr.max_length)
            ctx.actions.run(
                inputs = action_inputs,
                outputs = action_outputs,
                executable = ctx.file._extractor_tool,
                arguments = [args],
                progress_message = "Extracting text from files %d-%d" % (start, start + files_per_call),
            )
            outputs += action_outputs
    print(outputs)
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
        "files_per_call": attr.int(
            default = 8,
            doc = "Number of files to process in each invocation of the extractor tool.",
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
