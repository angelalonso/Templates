import argparse
import os
import textwrap


def parse_commandline_args():
    """Return parsed command line."""

    # Boilerplate: create a description text with proper indent.
    name = os.path.basename(__file__)
    description = textwrap.dedent(f'''
    Description goes here.

    Examples:
      {name} foo bar
    ''')

    # Instantiate the argument parser class.
    parser = argparse.ArgumentParser(
        description=description,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        add_help=False
    )

    # Specify the command line arguments. This one is a mandatory argument
    # because it does *not* start with a "-" (see next one for an optional
    # one).
    parser.add_argument(
        "foo", type=str, help="Mandatory argument"
    )

    # This is an optional argument (it starts with "-") and should have a
    # default values.
    parser.add_argument(
        "-bar", type=str, help="Optional argument", default="blah"
    )

    # Return the parse arguments in something akin to a named tuple.
    return parser.parse_args()


def main():
    param = parse_commandline_args()
    print(f"Foo={param.foo}")
    print(f"Bar={param.bar}")
    print(f"Entire structure: {param}")


if __name__ == '__main__':
    main()
