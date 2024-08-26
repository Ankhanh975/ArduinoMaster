def remove_duplicates_and_sort(file_path):
    # Read all lines from the file
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Use a set to track seen lines and filter out duplicates
    seen = set()
    unique_lines = []
    for line in lines:  
        if line not in seen:
            unique_lines.append(line)
            seen.add(line)

    # Sort the unique lines alphabetically
    unique_lines.sort()

    # Write the sorted unique lines back to the file
    with open(file_path, 'w') as file:
        file.writelines(unique_lines)

# Specify the path to the README.txt file
file_path = 'README.txt'
remove_duplicates_and_sort(file_path)
