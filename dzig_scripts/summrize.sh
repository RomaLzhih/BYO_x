#!/bin/bash

# Check if log file is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <log_file>"
    echo "Example: $0 experiment.log"
    exit 1
fi

LOG_FILE="$1"

if [ ! -f "$LOG_FILE" ]; then
    echo "Error: Log file '$LOG_FILE' not found!"
    exit 1
fi

# Parse the log file and extract data
awk '
BEGIN {
    current_graph = ""
    current_batch_size = ""
    current_algorithm = ""
    current_solver = ""
}

# Extract solver name and graph - look for pattern "Running <solver_name> on"
/Running .+ on/ {
    match($0, /Running ([^[:space:]]+) on ([^[:space:]]+)/, matches)
    current_solver = matches[1]
    current_graph = matches[2]
}

# Extract batch size
/Batch_size/ {
    match($0, /Batch_size ([0-9]+)/, batch_match)
    current_batch_size = batch_match[1]
}

# Extract algorithm
/Alg/ {
    match($0, /Alg ([^[:space:]]+)/, alg_match)
    current_algorithm = alg_match[1]
}

# Capture timing data (lines with two floating point numbers)
/^[0-9]+\.[0-9]+ [0-9]+\.[0-9]+$/ {
    if (current_graph != "" && current_batch_size != "" && current_algorithm != "" && current_solver != "") {
        key = current_solver ":" current_batch_size ":" current_algorithm ":" current_graph
        
        # Store the complete timing line
        if (timing_lines[key] == "") {
            timing_lines[key] = $0
        } else {
            timing_lines[key] = timing_lines[key] "\n" $0
        }
    }
}

END {
    # Group data by solver, batch_size, algorithm
    for (key in timing_lines) {
        split(key, parts, ":")
        solver = parts[1]
        batch_size = parts[2]
        algorithm = parts[3]
        graph = parts[4]
        
        group_key = solver ":" batch_size ":" algorithm
        if (!(group_key in groups)) {
            groups[group_key] = ""
            group_solver[group_key] = solver
            group_batch[group_key] = batch_size
            group_alg[group_key] = algorithm
        }
        
        groups[group_key] = groups[group_key] graph ":\n" timing_lines[key] "\n\n"
    }
    
    # Create sorted array of group keys
    n = 0
    for (group_key in groups) {
        sorted_keys[++n] = group_key
    }
    
    # Sort by solver name, then batch size (numeric), then algorithm
    for (i = 1; i <= n; i++) {
        for (j = i + 1; j <= n; j++) {
            split(sorted_keys[i], parts_i, ":")
            split(sorted_keys[j], parts_j, ":")
            
            solver_i = parts_i[1]
            solver_j = parts_j[1]
            batch_i = parts_i[2] + 0  # Convert to number
            batch_j = parts_j[2] + 0  # Convert to number
            alg_i = parts_i[3]
            alg_j = parts_j[3]
            
            # Sort by solver first, then batch size, then algorithm
            should_swap = 0
            if (solver_i > solver_j) {
                should_swap = 1
            } else if (solver_i == solver_j) {
                if (batch_i > batch_j) {
                    should_swap = 1
                } else if (batch_i == batch_j && alg_i > alg_j) {
                    should_swap = 1
                }
            }
            
            if (should_swap) {
                temp = sorted_keys[i]
                sorted_keys[i] = sorted_keys[j]
                sorted_keys[j] = temp
            }
        }
    }
    
    # Output in the requested format with sorted order
    for (i = 1; i <= n; i++) {
        group_key = sorted_keys[i]
        
        print group_solver[group_key]
        print "batch size " group_batch[group_key]
        print group_alg[group_key]
        printf "%s", groups[group_key]
    }
}
' "$LOG_FILE"
