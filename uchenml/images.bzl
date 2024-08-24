""" Rules for image processing. """

def _remove_extension(path):
    return path[:path.rfind(".")]

def _pad(n, length):
    return "0" * (length - len(str(n))) + str(n)

def _build_ops(ctx, rule_name, src, base):
    # Resize maintaining the aspect ratio.
    resize1_args = ctx.actions.args()
    resize1_args.add("-density", "300")
    resize1_args.add("-resize", "%sx%s" % (ctx.attr.width, ctx.attr.height))
    resize1_args.add(src)
    temp_file = ctx.actions.declare_file("%s/temp/%s.png" % (rule_name, base))
    resize1_args.add(temp_file.path)
    ctx.actions.run(
        executable = "convert",
        inputs = [src],
        outputs = [temp_file],
        arguments = [resize1_args],
    )
    # Add empty space to make the image expected size.
    resize2_args = ctx.actions.args()
    resize2_args.add("-gravity", "center")
    resize2_args.add("-extent", "%sx%s" % (ctx.attr.width, ctx.attr.height))
    resize2_args.add(temp_file.path)
    output = ctx.actions.declare_file("%s/%s.png" % (rule_name, base))
    resize2_args.add(output)
    ctx.actions.run(
        executable = "convert",
        inputs = [temp_file],
        outputs = [output],
        arguments = [resize2_args],
    )
    return [output]

def _resize_image_impl(ctx):
    # This is a placeholder implementation.
    # In a real implementation, we would resize the image.
    outputs = []
    i = 1
    pad_length = len(str(len(ctx.files.srcs)))
    for src in ctx.files.srcs:
        base = "%s-%s" % (_pad(i, pad_length), _remove_extension(src.basename))
        outputs = outputs + _build_ops(ctx, ctx.attr.name, src, base)

    return DefaultInfo(files = depset(outputs))

# convert -density 300 test/rules/uchen-logo.svg -resize 600x600^ output.png
# convert output.png -gravity center -extent 530x530 final_output.png

resize_image = rule(
    implementation = _resize_image_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "The input images to resize.",
        ),
        "width": attr.int(
            doc = "The width of the resized image.",
        ),
        "height": attr.int(
            doc = "The height of the resized image.",
        ),
    },
    doc = "Resizes images to a specified width and height.",
)
