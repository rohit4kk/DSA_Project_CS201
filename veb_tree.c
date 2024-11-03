#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h> // For sleep function
#include <stdbool.h>

// Define Van Emde Boas Tree structure
typedef struct vEBTree
{
    int u;
    int min;
    int max;
    struct vEBTree *summary;
    struct vEBTree **clusters;
} vEBTree;

// Function to return cluster number of x
int high(vEBTree *vEB, int x)
{
    int div = (int)ceil(sqrt(vEB->u));
    return x / div;
}

// Function to return position of x in cluster
int low(vEBTree *vEB, int x)
{
    int mod = (int)ceil(sqrt(vEB->u));
    return x % mod;
}

// Function to return the index from cluster number and position
int generate_index(vEBTree *vEB, int x, int y)
{
    int ru = (int)ceil(sqrt(vEB->u));
    return x * ru + y;
}

// Function to create and initialize a Van Emde Boas Tree
vEBTree *create_vEB(int size)
{
    if (size <= 0)
        return NULL;

    vEBTree *vEB = (vEBTree *)malloc(sizeof(vEBTree));
    vEB->u = size;
    vEB->min = -1;
    vEB->max = -1;

    if (size <= 2)
    {
        vEB->summary = NULL;
        vEB->clusters = NULL;
    }
    else
    {
        int num_clusters = (int)ceil(sqrt(size));
        vEB->summary = create_vEB(num_clusters);
        vEB->clusters = (vEBTree **)malloc(num_clusters * sizeof(vEBTree *));
        for (int i = 0; i < num_clusters; i++)
        {
            vEB->clusters[i] = create_vEB((int)ceil(sqrt(size)));
        }
    }
    return vEB;
}

// Minimum and maximum functions for Van Emde Boas tree
int vEB_min(vEBTree *vEB)
{
    if (vEB)
        return vEB->min;
    return -1;
}

int vEB_max(vEBTree *vEB)
{
    return (vEB->max == -1 ? -1 : vEB->max);
}

// Key insertion for traffic congestion management
void insert(vEBTree *vEB, int key)
{
    if (!vEB)
        return;

    if (vEB->min == -1)
    {
        vEB->min = key;
        vEB->max = key;
    }
    else
    {
        if (key < vEB->min)
        {
            int temp = vEB->min;
            vEB->min = key;
            key = temp;
        }

        if (vEB->u > 2)
        {
            if (vEB_min(vEB->clusters[high(vEB, key)]) == -1)
            {
                insert(vEB->summary, high(vEB, key));
                vEB->clusters[high(vEB, key)]->min = low(vEB, key);
                vEB->clusters[high(vEB, key)]->max = low(vEB, key);
            }
            else
            {
                insert(vEB->clusters[high(vEB, key)], low(vEB, key));
            }
        }

        if (key > vEB->max)
        {
            vEB->max = key;
        }
    }
}

// Function to check if a key is present in the tree
int isMember(vEBTree *vEB, int key)
{
    if (!vEB || key >= vEB->u)
        return 0;

    if (vEB->min == key || vEB->max == key)
    {
        return 1;
    }
    else if (vEB->u == 2)
    {
        return 0;
    }
    else
    {
        return isMember(vEB->clusters[high(vEB, key)], low(vEB, key));
    }
}

// Function to find the successor of a given key
int vEB_successor(vEBTree *vEB, int key)
{
    if (!vEB)
        return -1;

    if (vEB->u == 2)
    {
        if (key == 0 && vEB->max == 1)
            return 1;
        else
            return -1;
    }
    else if (vEB->min != -1 && key < vEB->min)
    {
        return vEB->min;
    }
    else
    {
        int max_incluster = vEB_max(vEB->clusters[high(vEB, key)]);
        if (max_incluster != -1 && low(vEB, key) < max_incluster)
        {
            int offset = vEB_successor(vEB->clusters[high(vEB, key)], low(vEB, key));
            return generate_index(vEB, high(vEB, key), offset);
        }
        else
        {
            int succ_cluster = vEB_successor(vEB->summary, high(vEB, key));
            if (succ_cluster == -1)
                return -1;
            int offset = vEB_min(vEB->clusters[succ_cluster]);
            return generate_index(vEB, succ_cluster, offset);
        }
    }
}

// Function to delete a point from the Van Emde Boas tree
void vEB_delete(vEBTree *veb, int key)
{
    if (!veb || key >= veb->u)
        return;

    if (veb->min == veb->max)
    {
        veb->min = -1;
        veb->max = -1;
    }
    else if (veb->u == 2)
    {
        if (key == 0)
            veb->min = 1;
        else
            veb->min = 0;
        veb->max = veb->min;
    }
    else
    {
        if (key == veb->min)
        {
            int first_cluster = vEB_min(veb->summary);
            key = generate_index(veb, first_cluster, vEB_min(veb->clusters[first_cluster]));
            veb->min = key;
        }

        vEB_delete(veb->clusters[high(veb, key)], low(veb, key));

        if (vEB_min(veb->clusters[high(veb, key)]) == -1)
        {
            vEB_delete(veb->summary, high(veb, key));
            if (key == veb->max)
            {
                int max_in_summary = vEB_max(veb->summary);
                veb->max = (max_in_summary == -1) ? veb->min : generate_index(veb, max_in_summary, vEB_max(veb->clusters[max_in_summary]));
            }
        }
        else if (key == veb->max)
        {
            veb->max = generate_index(veb, high(veb, key), vEB_max(veb->clusters[high(veb, key)]));
        }
    }
}

// Cleanup function to free memory allocated to Van Emde Boas tree
void free_vEB(vEBTree *vEB)
{
    if (!vEB)
        return;

    if (vEB->clusters)
    {
        int num_clusters = (int)ceil(sqrt(vEB->u));
        for (int i = 0; i < num_clusters; i++)
        {
            free_vEB(vEB->clusters[i]);
        }
        free(vEB->clusters);
    }

    free_vEB(vEB->summary);
    free(vEB);
}

//                      TRAFFIC CONGESTION ALERT IMPLEMENTATION

#define Max_distance 30000
#define min_dist 1000
#define max_dist 3000

// Define a point with only an x-coordinate and speed
typedef struct Point
{
    int x;
    int speed;
} Point;

// Function to load points from input file
void load_points(const char *filename, Point points[], int *n)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        exit(1);
    }

    *n = 0;
    while (fscanf(file, "%d %d", &points[*n].x, &points[*n].speed) != EOF && *n < 100)
    {
        (*n)++;
    }

    fclose(file);
}

// Function to count vehicles within a specific range of distances using successor function in Van Emde Boas tree
int count_vehicles_in_range(vEBTree *veb_tree, int input_distance, int min_distance, int max_distance)
{
    if (!veb_tree)
        return 0;
    int count = 0;

    // Calculate the actual distances from input_x to search within
    int lower_bound = input_distance + min_distance;
    int upper_bound = input_distance + max_distance;

    // Start finding successors from the lower_bound
    int current_distance = vEB_successor(veb_tree, lower_bound - 1);

    while (current_distance != -1 && current_distance <= upper_bound)
    {
        count++;
        current_distance = vEB_successor(veb_tree, current_distance);
    }

    return count;
}

// Function to print all elements in the Van Emde Boas tree in sorted order
void print_all_elements(vEBTree *vEB)
{
    if (!vEB)
        return;
    int current = vEB_min(vEB); // Start from the minimum element

    // Traverse through all elements using successor function
    while (current != -1)
    {
        printf("%d ", current);                // Print the current element
        current = vEB_successor(vEB, current); // Move to the next element
    }
    printf("\n"); // Newline after printing all elements
}

int main()
{

    Point points[100]; // Assuming a max of 100 vehicles
    int num_points;

    // Load points from input file
    load_points("input.txt", points, &num_points);
    printf("Number of vehicles loaded: %d\n", num_points);

    float input_x;
    int input_speed;
    printf("Enter the x_coordinate(in km) of vehicle to check for congestion between 0 to 30 km: ");

    scanf("%f", &input_x);
    printf("Enter speed(m/s) of vehicle whose coordinate you have given: ");
    scanf("%d", &input_speed);

    int congestion_threshold;
    printf("Enter the Congestion threshold: ");
    scanf("%d", &congestion_threshold);

    // Check congestion after updating positions
    int input_time;
    printf("Enter for how many seconds you want to run the program: ");
    scanf("%d", &input_time);

    input_x *= 1000;                 // convert km to m
    bool deleted_points[num_points]; // To keep track of deleted points
    for (int i = 0; i < num_points; i++)
    {
        deleted_points[i] = false;
    }

    // Open delete file initially in write mode to create a new file for this run
    FILE *delete_file = fopen("delete_output.txt", "w");
    if (!delete_file)
    {
        perror("Error opening delete_output.txt");
        exit(1);
    }

    int t = 0;
    int remaining_points = num_points;
    while (t < input_time)
    {
        vEBTree *veb_tree = create_vEB(32768); // Assuming at most 30 km
        for (int i = 0; i < num_points; i++)
        {
            if (points[i].x <= Max_distance)
            {
                insert(veb_tree, points[i].x);
            }
            else if (!deleted_points[i])
            {
                fprintf(delete_file, "Vehicle at position %d exceeded maximum distance and was removed at time %d.\n", points[i].x, t);
                deleted_points[i] = true;
                remaining_points--; // Decrease the number of points
            }
        }

        int count = count_vehicles_in_range(veb_tree, input_x, min_dist, max_dist);
        printf("Number of vehicles in range: %d\n", count);
        if (count >= congestion_threshold)
        {
            printf("Congestion alert\n");
        }
        else
        {
            printf("No congestion\n");
        }

        printf("\n");

        if (t == input_time - 1)
        {
            printf("Number of vehicles remaining: %d\n", remaining_points);
            printf("Remaining elements in the vEB Tree:\n");
            print_all_elements(veb_tree);
        }

        free_vEB(veb_tree);

        // Update positions of all vehicles
        for (int i = 0; i < num_points; i++)
        {
            points[i].x += points[i].speed;
        }
        input_x += input_speed;

        t++;
        sleep(1);
    }

    // Clean up and close the file
    fclose(delete_file);
    return 0;
}