"""
Helper script to convert class objects into dictionaries for comparison
"""


def to_dict(item):
    """
    Convert class object to dict
    """
    response_dict = {}
    # case when item is list of object
    if isinstance(item, list):
        for i in item:
            response_dict.update(to_dict(i))
    else:
        for attr, value in vars(item).items():
            if isinstance(value, list):
                value_list = []
                for i in value:
                    if isinstance(i, (str, int, float, bool)):
                        value_list.append(i)
                    else:
                        value_list.append(i.__dict__)
                response_dict.update({str(attr): value_list})
            elif isinstance(value, (str, int, float, dict, bool)) or value is None:
                response_dict.update({str(attr): value})
            # case when the value is an object; inspect.isclass() doesn't work
            else:
                response_dict.update({str(attr): to_dict(value)})
    return response_dict
