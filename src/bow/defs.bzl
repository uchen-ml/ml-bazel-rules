"""Starlark rules for Uchen text processing."""

# The script content, with placeholders for the data file name and number of
# haikus.
script_template = """\
#!/bin/bash
echo Boop!
"""


def _foo_binary_impl(ctx):
    runfiles = ctx.runfiles(files = [ctx.attr._generator.files_to_run.executable])
    runfiles = runfiles.merge(ctx.attr._generator[DefaultInfo].default_runfiles)
    script = ctx.actions.declare_file("%s-launcher.sh" % ctx.label.name)
    ctx.actions.run(
        executable = ctx.attr._generator.files_to_run.executable,
        outputs = [script],
    )
    return [DefaultInfo(files = depset([script]), runfiles = runfiles)]

uchen_wordbag = rule(
    implementation = _foo_binary_impl,
    attrs = {
        "_generator": attr.label(
            default = Label("//tokens:tokens"),
            executable = True,
            allow_single_file = True,
            cfg = "exec",
        ),
    },
)