/***
Output file structure:

The voxel grid (three dimensional) is represented as a stack of two dimensional grids. 

First, three (integer) values are given:
 - Number of voxels along the X dimension.
 - Number of voxels along the Y dimension.
 - Number of voxels along the Z dimension.

Then, the values composing the voxel grid are given (double).
Ogni sequenza di lunghezza pari al numero di voxel lungo l'asse X moltiplicata per il numero di veoxel lungo l'asse Z rappresenta una griglia.
The order followed in writing the two-dimensional grids is starting from the bottom one.

*/

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "voxel.h"

#define OBJ_BUFFER 1000
#define DEFAULT_WORK_SIZE 2352
#define N_PIXEL_ALONG_SIDE (DETECTOR_SIDE_LENGTH / PIXEL_DIM)

int gl_pixelDim = PIXEL_DIM;
int gl_angularTrajectory = ANGULAR_TRAJECTORY;
int gl_positionsAngularDistance = POSITIONS_ANGULAR_DISTANCE;
int gl_objectSideLenght;
int gl_detectorSideLength;
int gl_distanceObjectDetector;
int gl_distanceObjectSource;
int gl_voxelXDim = VOXEL_X_DIM;
int gl_voxelYDim = VOXEL_Y_DIM;
int gl_voxelZDim = VOXEL_Z_DIM;
int gl_nVoxel[3];
int gl_nPlanes[3];


int main(int argc, char *argv[])
{

    FILE* filePoiter;
    char* fileName = "output.dat";
    int n = DEFAULT_WORK_SIZE;                  // number of voxel along the detector's side, 
                                                //furthermore the environment parmeters are computed in function of 'n'.                  
    
    int objectType = 0;                         // represents the object type chosen

    if(argc > 4 || argc < 1){
        fprintf(stderr,"Usage:\n\t%s output.dat [integer] [object Type]\n - First parameter is the name of the file to store the output in; "
                        "\n - Second parameter is the number of pixel per side of the detector, if not given 2352 is default; "
                        "\n - Third parameter is optional and can be: 1 (solid cube with spherical cavity), 2 (solid sphere) or 3 (solid cube), "
                        "if not passed 3 (solid cube) is default.\n",argv[0]);

        return EXIT_FAILURE;
    }
    if(argc > 1){
        fileName = argv[1];
    }
    if(argc > 2){
        n = atoi(argv[2]);
    }
    if(argc > 3){
        objectType = atoi(argv[3]);
    }

    //global variables set-up
    gl_objectSideLenght = n * gl_voxelXDim * ((double)OBJECT_SIDE_LENGTH / (VOXEL_X_DIM * N_PIXEL_ALONG_SIDE));
    gl_detectorSideLength = n * gl_pixelDim;
    gl_distanceObjectDetector = 1.5 * gl_objectSideLenght;
    gl_distanceObjectSource = 6 * gl_objectSideLenght;

    gl_nVoxel[X] = gl_objectSideLenght / gl_voxelXDim;
    gl_nVoxel[Y] = gl_objectSideLenght / gl_voxelYDim;
    gl_nVoxel[Z] = gl_objectSideLenght / gl_voxelZDim;

    gl_nPlanes[X] = (gl_objectSideLenght / gl_voxelXDim) + 1;
    gl_nPlanes[Y] = (gl_objectSideLenght / gl_voxelYDim) + 1;
    gl_nPlanes[Z] = (gl_objectSideLenght / gl_voxelZDim) + 1;

    //array containing the coefficents of each voxel
    double *grid = (double*)malloc(sizeof(double) * gl_nVoxel[X] * gl_nVoxel[Z] * OBJ_BUFFER);
    //array containing the computed absorption detected in each pixel of the detector


    //Open File
    filePoiter = fopen(fileName,"wb");
    if (!filePoiter){
        printf("Unable to open file!");
        exit(2);
    }

    //Write the voxel grid dimensions on file
    if(!fwrite(gl_nVoxel, sizeof(int), 3, filePoiter)){
        printf("Unable to write on file!");
        exit(3);
    }


    //iterates over object subsection
    for(int slice = 0; slice < gl_nVoxel[Y]; slice += OBJ_BUFFER){
        //generate object subsection
        switch (objectType){
            case 1:
                generateCubeWithSphereSlice(grid, OBJ_BUFFER, slice, gl_nVoxel[X]);
                break;
            case 2:
                generateSphereSlice(grid, OBJ_BUFFER, slice, gl_objectSideLenght / 2);
                break;
            default:
                generateCubeSlice(grid, OBJ_BUFFER, slice, gl_nVoxel[X]);
                break;
        }

        if(slice < gl_nVoxel[Y]){
            printf("Size: %lu\n",sizeof(double) * gl_nVoxel[X] * gl_nVoxel[Z] * OBJ_BUFFER);
            if(!fwrite(grid, sizeof(double), gl_nVoxel[X] * gl_nVoxel[Z] * OBJ_BUFFER, filePoiter)){
                printf("Unable to write on file!");
                exit(4);
            }
        } else {
            printf("Size: %lu\n",sizeof(double) * gl_nVoxel[X] * gl_nVoxel[Z] * (slice - gl_nVoxel[Y]));
            if(!fwrite(grid, sizeof(double), gl_nVoxel[X] * gl_nVoxel[Z] * (slice - gl_nVoxel[Y]), filePoiter)){
                printf("Unable to write on file!");
                exit(5);
            }
        }
    }

    fclose(filePoiter);
    free(grid);

}