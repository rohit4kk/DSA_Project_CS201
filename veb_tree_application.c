#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Define Van Emde Boas Tree structure
typedef struct vEBTree {
    int u;
    int min;
    int max;
    struct vEBTree* summary;
    struct vEBTree** clusters;
} vEBTree;

// Function to return cluster number of x
int high(vEBTree* vEB, int x){
    int div = (int)ceil(sqrt(vEB->u));
    return x / div;
}

// Function to return postition of x in cluster
int low(vEBTree* vEB, int x) {
    int mod = (int)ceil(sqrt(vEB->u));
    return x % mod;
}

// Function to return the index from cluster number and position
int generate_index(vEBTree* vEB, int x, int y) {
    int ru = (int)ceil(sqrt(vEB->u));
    return x * ru + y;
}

// Function to create and initialize a Van Emde Boas Tree
vEBTree* create_vEB(int size) {
    vEBTree* vEB = (vEBTree*)malloc(sizeof(vEBTree));
    vEB->u = size;
    vEB->min = -1;
    vEB->max = -1;

    if (size <= 2) {
        vEB->summary = NULL;
        vEB->clusters = NULL;
    } else {
        int num_clusters = (int)ceil(sqrt(size));
        vEB->summary = create_vEB(num_clusters);
        vEB->clusters = (vEBTree**)malloc(num_clusters * sizeof(vEBTree*));
        for (int i = 0; i < num_clusters; i++) {
            vEB->clusters[i] = create_vEB((int)ceil(sqrt(size)));
        }
    }
    return vEB;
}

 // vEBTree returns min and max in O(1) time

int vEB_min(vEBTree* vEB) {
    return (vEB->min == -1 ? -1 : vEB->min);
}

int vEB_max(vEBTree* vEB) {
    return (vEB->max == -1 ? -1 : vEB->max);
}


// key will be a integer in this case as we want to do manage traffic congestion using this tree
void insert(vEBTree* vEB, int key) {
    if (vEB->min == -1) {
        vEB->min = key;
        vEB->max = key;
    } else {
        if (key < vEB->min) {
            int temp = vEB->min;
            vEB->min = key;
            key = temp;
        }

        if (vEB->u > 2) {
            if (vEB_min(vEB->clusters[high(vEB, key)]) == -1) {
                insert(vEB->summary, high(vEB, key));
                vEB->clusters[high(vEB, key)]->min = low(vEB, key);
                vEB->clusters[high(vEB, key)]->max = low(vEB, key);
            } else {
                insert(vEB->clusters[high(vEB, key)], low(vEB, key));
            }
        }

        if (key > vEB->max) {
            vEB->max = key;
        }
    }
}

// Function to check if the key is present in the tree
// after checking this only we perform
int isMember(vEBTree* vEB, int key) {
    if (vEB->u < key) {
        return 0;
    }

    if (vEB->min == key || vEB->max == key) {
        return 1;
    } else {
        if (vEB->u == 2) {
            return 0;
        } else {
            return isMember(vEB->clusters[high(vEB, key)], low(vEB, key));
        }
    }
}

// Function to find the successor of a given key
// it gives successor in log(log(u)) time complexity
int vEB_successor(vEBTree* vEB, int key) {
    if (vEB->u == 2) {
        if (key == 0 && vEB->max == 1) {
            return 1;
        } else {
            return -1;
        }
    } else if (vEB->min != -1 && key < vEB->min) {
        return vEB->min;
    } else {
        int max_incluster = vEB_max(vEB->clusters[high(vEB, key)]);

        if (max_incluster != -1 && low(vEB, key) < max_incluster) {
            int offset = vEB_successor(vEB->clusters[high(vEB, key)], low(vEB, key));
            return generate_index(vEB, high(vEB, key), offset);
        } else {
            int succ_cluster = vEB_successor(vEB->summary, high(vEB, key));
            if (succ_cluster == -1) {
                return -1;
            } else {
                int offset = vEB_min(vEB->clusters[succ_cluster]);
                return generate_index(vEB, succ_cluster, offset);
            }
        }
    }
}

// Function to find the predecessor of a given key
int vEB_predecessor(vEBTree* vEB, int key) {
    if (vEB->u == 2) {
        if (key == 1 && vEB->min == 0) {
            return 0;
        } else {
            return -1;
        }
    } else if (vEB->max != -1 && key > vEB->max) {
        return vEB->max;
    } else {
        int min_incluster = vEB_min(vEB->clusters[high(vEB, key)]);
        if (min_incluster != -1 && low(vEB, key) > min_incluster) {
            int offset = vEB_predecessor(vEB->clusters[high(vEB, key)], low(vEB, key));
            return generate_index(vEB, high(vEB, key), offset);
        } else {
            int pred_cluster = vEB_predecessor(vEB->summary, high(vEB, key));
            if (pred_cluster == -1) {
                if (vEB->min != -1 && key > vEB->min) {
                    return vEB->min;
                } else {
                    return -1;
                }
            } else {
                int offset = vEB_max(vEB->clusters[pred_cluster]);
                return generate_index(vEB, pred_cluster, offset);
            }
        }
    }
}

// perform deletion in O(log(logu))
void vEB_delete(vEBTree* veb, int key)
{
    if (veb->max == veb->min) {
        veb->min = -1;
        veb->max = -1;
    }
    else if (veb->u == 2) {
        if (key == 0) {
            veb->min = 1;
        } else {
            veb->min = 0;
        }
        veb->max = veb->min;
    }
    else {
        if (key == veb->min) {
            int first_cluster = vEB_min(veb->summary);
            key = generate_index(veb, first_cluster,vEB_min(veb->clusters[first_cluster]));
            veb->min = key;
        }

        vEB_delete(veb->clusters[high(veb, key)], low(veb, key));

        if (vEB_min(veb->clusters[high(veb, key)]) == -1) {
            vEB_delete(veb->summary, high(veb, key));

            if (key == veb->max) {
                int max_in_summary = vEB_max(veb->summary);

                if (max_in_summary == -1) {
                    veb->max = veb->min;
                } else {
                    veb->max = generate_index(veb, max_in_summary,
                        vEB_max(veb->clusters[max_in_summary]));
                }
            }
        }
        else if (key == veb->max) {
            veb->max = generate_index(veb, high(veb, key),vEB_max(veb->clusters[high(veb, key)]));
        }
    }
}

//         TRAFFIC CONGESTION ALERT USING vEB TREE


#define THRESHOLD_DISTANCE 40000  // 40 km in meters


typedef struct Point {
    int x;
    int y;
    int speed;
} Point;

// Function to calculate distance from (0,0)
double calculate_distance(int x, int y) {
    return sqrt(x * x + y * y);
}

// Function to load points from input file
void load_points(const char *filename, Point points[], int *n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    *n = 0;
    while (fscanf(file, "%d %d %d", &points[*n].x, &points[*n].y, &points[*n].speed) != EOF) {
        (*n)++;
    }

    fclose(file);
}

// Function to update point positions based on speed
void update_points(Point points[], int n) {
    for (int i = 0; i < n; i++) {
        points[i].x += points[i].speed;
        points[i].y += points[i].speed;
    }
}

// Function to check points and update delete_input file if they exceed distance
void check_points(Point points[], int n, const char *delete_filename) {
    FILE *delete_file = fopen(delete_filename, "w");
    if (!delete_file) {
        perror("Error opening delete_input file");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        double distance = calculate_distance(points[i].x, points[i].y);
        if (distance > THRESHOLD_DISTANCE) {
            // Add point to delete_input file
            fprintf(delete_file, "%d %d %d\n", points[i].x, points[i].y, points[i].speed);
        }
    }

    fclose(delete_file);
}

// Function to delete points from Van Emde Boas tree (stub for now)
void delete_points_from_tree(vEBTree* veb,const char *delete_filename) {
    FILE *delete_file = fopen(delete_filename, "r");
    if (!delete_file) {
        perror("Error opening delete_input file for reading");
        exit(1);
    }

    int x, y, speed;
    while (fscanf(delete_file, "%d %d %d", &x, &y, &speed) != EOF) {
        int key=calculate_distance(x,y);
        vEB_delete(veb,key);
        printf("Deleted (%d, %d, %d) from tree\n", x, y, speed);
    }

    fclose(delete_file);
}


// Function to count vehicles within a radius using the successor function of Van Emde Boas tree
int count_vehicles_around(vEBTree* veb_tree, int distance, int radius) {
    int count = 0;
    int current_distance = VEB_successor(veb_tree, distance - radius);

    while (current_distance != -1 && current_distance <= distance + radius) {
        count++;
        current_distance = VEB_successor(veb_tree, current_distance);
    }

    return count; 
}

int main() {
    Point points[100];  // Assuming a max of 100 points
    int num_points;
    vEBTree* veb_tree = create_VEB_tree(1000);  // assuming atmost 1000 vehicles
    int congestion_threshold = 10;
    int radius = 5;  // Define the search radius for congestion detection

    load_points("input.txt", points, &num_points);
    int t=10;
    while (t--) {
        // Update points every 5 seconds
        sleep(5);

        // Update point positions based on speed
        update_points(points, num_points);

        // Insert distances into the Van Emde Boas tree
        for (int i = 0; i < num_points; ++i) {
            Point p=points[i];
            int distance = calculate_distance(p.x,p.y);
            VEB_insert(veb_tree, distance);
        }

        // Check points and write to delete_input file if distance exceeds threshold
        check_points(points, num_points, "delete_input.txt");

        // Delete points from Van Emde Boas tree
        delete_points_from_tree(veb_tree,"delete_input.txt");

        // Check for congestion based on successor function
        int input_x, input_y;
        printf("Enter coordinates (x y) to check for congestion: ");
        scanf("%d %d", &input_x, &input_y);

        // Calculate distance from reference point (0, 0) to the input coordinates
        int input_distance = calculate_distance(input_x,input_y);

        // Find how many vehicles are within the radius of the input distance
        int vehicle_count = count_vehicles_around(veb_tree, input_distance, radius);

        // Congestion alert based on the threshold
        if (vehicle_count > congestion_threshold) {
            printf("Congestion alert: %d vehicles found within %d units of distance!\n", vehicle_count, radius);
        } else {
            printf("No congestion: %d vehicles found.\n", vehicle_count);
        }

        // Repeat every 5 seconds
    }

    return 0;
}
