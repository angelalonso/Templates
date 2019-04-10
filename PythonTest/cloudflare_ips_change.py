"""Alert when the result from GETting a given URL changes

Use case:
This script is built to check the list of IPs Cloudflare uses.
We would need to update some security groups whenever that list changes.

It runs on a loop and does nothing if nothing changes.
If there is any change, it freezes until you decide to update the "expected" result
"""
import requests
import sys


def get_current(url):
    """ Gets the current result"""
    return requests.get(url).json()


def load_file(filestore):
    """ Checks if we have a previous result stored in a file
      - if it exists, it returns its contents
      - if it doesn't, it returns a FileNotFoundError string
      """
    try:
        with open(filestore, "r") as fileloaded:
            data = fileloaded.readlines()
        return "", ''.join(data)
    except FileNotFoundError:
        return "FileNotFoundError", ""


def update_file(filestore, content):
    """ Overwrites the file with a given content"""
    filetowrite = open(filestore, "w")
    filetowrite.write(content)
    filetowrite.close()


def first_run(url, expected, filestore):
    """ As a first run,
         we get fresh data from the URL and write it to the file right away"""
    expected = get_current(url).decode("utf-8")
    update_file(filestore, expected)


def compare(expected, filestore):
    """ Compares what we got from the URL with what we loaded in our 'EXPECTED' variable
          , which comes either from the file or a previous run"""
    result = get_current(url)
    if result != expected:
        alert(expected, result)
        decision_making(result, filestore)


def alert(expected, result):
    """ Shows an alert with both what we got and what we expected to get from the URL"""
    print("something changed!")
    print("We had:")
    print(expected)
    print("We got:")
    print(result)


def decision_making(result, filestore):
    """ Asks the user for confirmation on whether to update what we expect or not"""
    correct_answer = False
    while not correct_answer:
        query = input("Do you want to update the expected result with the latest?")
        if query == '':
            print('Please answer with yes or no!')
        else:
            answer = query[0].lower()
            if answer == 'y':
                correct_answer = True
                update_file(filestore, result)
            elif answer == 'n':
                correct_answer = True
            else:
                print('Please answer with yes or no!')


def help():
    """ Self explanatory"""
    print("SYNTAX:\n")
    print(sys.argv[0] + " http://your.url\n")


if __name__ == '__main__':
    """ We make sure we actually get a parameter that should contain the URL
          (we don't check that)
        , then we look for a file containing previous results, load it if present
        , and look for changes on a loop
    """
    filestore = "./expected.out"
    expected = ""

    if len(sys.argv) < 2:
        help()
        sys.exit(2)
    url = sys.argv[1]
    while True:
        fileerr, expected = load_file(filestore)
        if fileerr == "FileNotFoundError":
            first_run(url, expected, filestore)
        else:
            compare(expected, filestore)
