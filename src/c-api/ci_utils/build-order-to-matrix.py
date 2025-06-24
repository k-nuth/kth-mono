import json
from copy import deepcopy

def main():
    matrix = {"config": []}

    with open("platform.json", "r") as platform_file:
        platform_data = json.load(platform_file)

        with open("build_order.json", "r") as read_file:
            data = json.load(read_file)
            for level in data:
                # print(f"Level: {level}")
                for reference in level:
                    print(f"reference: {reference['ref']}")
                    for platform in platform_data['config']:
                        platform_final = deepcopy(platform)
                        platform_final["name"] = f'{platform_final["name"]} - {reference["ref"]}'
                        platform_final["reference"] = reference["ref"]
                        # print(f"reference: {platform['reference']}")
                        matrix["config"].append(deepcopy(platform_final))

            if len(matrix["config"]) == 0:
                matrix["config"].append({"reference": "null"})

    # print(matrix)
    with open("matrix.json", "w") as write_file:
        json.dump(matrix, write_file)
        write_file.write("\n")

if __name__ == "__main__":
    main()
