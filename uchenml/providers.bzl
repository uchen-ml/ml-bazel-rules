""" Providers for the dataset module. """

DatasetInfo = provider(
    doc = "Information about a dataset.",
    fields = {
        "data": "Directory containing the dataset.",
    },
)
