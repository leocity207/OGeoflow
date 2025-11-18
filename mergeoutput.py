import os

def collect_and_concatenate(source_dir, output_file, excluded_dirs=None):
    if excluded_dirs is None:
        excluded_dirs = []

    # Normalize excluded directory names
    excluded_dirs = set(os.path.normpath(d) for d in excluded_dirs)

    valid_extensions = {".txt", ".cmake"}
    collected_files = []

    for root, dirs, files in os.walk(source_dir):
        # Skip excluded directories
        dirs[:] = [d for d in dirs if os.path.normpath(os.path.join(root, d)) not in
                   {os.path.normpath(os.path.join(source_dir, ed)) for ed in excluded_dirs}]

        for file in files:
            if os.path.splitext(file)[1] in valid_extensions:
                full_path = os.path.join(root, file)
                collected_files.append(full_path)

    with open(output_file, 'w', encoding='utf-8') as out:
        for filepath in collected_files:
            relative_path = os.path.relpath(filepath, source_dir)
            out.write(f"\n*** {relative_path} ***\n")
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as src:
                out.write(src.read())
                out.write('\n')

    print(f"Concatenation complete. {len(collected_files)} files written to '{output_file}'.")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Concatenate all .h/.cpp files into one file.")
    parser.add_argument("source_dir", help="Path to the source directory.")
    parser.add_argument("output_file", help="Path to the output file.")
    parser.add_argument("--exclude", nargs='*', default=[], help="List of folders to exclude.")

    args = parser.parse_args()
    collect_and_concatenate(args.source_dir, args.output_file, excluded_dirs=args.exclude)
