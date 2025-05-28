#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <stdbool.h>  

//string 2D array "3x4:1,2,3,4,5,6,7,8,9,10,11,12"
void parse2DArray(int* g_width, int* g_height, char *strArray, int*** arrayPtr) {
	int width, height;
	char* token = strtok(strArray, "x");
	if(token == NULL) {
		fprintf(stderr, "parse2DArray -> bad 2d array width"); 
		exit(EXIT_FAILURE);
	}
	
	printf("token %s \n", token);
	width = atoi(token); 
	token = strtok(NULL, ":");
	if(token == NULL) {
		fprintf(stderr, "parse2DArray -> bad 2d array height"); 
		exit(EXIT_FAILURE);
	}
	height = atoi(token);

	*g_width = width;
	*g_height = height; 


	*arrayPtr = malloc(sizeof(int*)*height);
	for(int i = 0; i<height; i++) {
		(*arrayPtr)[i] = malloc(sizeof(int)*width);
		for(int j = 0; j<width; j++) {
			token = strtok(NULL, ",");	
			if(token == NULL) {
				fprintf(stderr, "parse2DArray -> bad 2d array data"); 
				exit(EXIT_FAILURE);
			}
			(*arrayPtr)[i][j] = atoi(token); 
		}
	}
} 

//string vector3 "0.0,0.0,0.0"
void parseVector3(char* strArray, Vector3* vector3) {
	char* token = strtok(strArray, ",");	
	if(token == NULL) {
                fprintf(stderr, "parseVector3 -> no x");
                exit(EXIT_FAILURE);
        }
	vector3->x = atof(token);
	token = strtok(NULL, ",");
	if(token == NULL) {
                fprintf(stderr, "parseVector3 -> no y");
                exit(EXIT_FAILURE);
        }
	vector3->y = atof(token);
	if(token == NULL) {
                fprintf(stderr, "parse2DArray -> no z"); 
                exit(EXIT_FAILURE);
        }
	token = strtok(NULL, ",");
	vector3->z = atof(token);
}

void parseColor(char* strArray, Color* vector3) {
        char* token = strtok(strArray, ",");
        if(token == NULL) {
                fprintf(stderr, "parseColor -> no x");
                exit(EXIT_FAILURE);
        }
        vector3->r = atof(token);
        token = strtok(NULL, ",");
        if(token == NULL) {
                fprintf(stderr, "parseColor -> no y");
                exit(EXIT_FAILURE);
        }
        vector3->g = atof(token);
        if(token == NULL) {
                fprintf(stderr, "parse2DArray -> no z");
                exit(EXIT_FAILURE);
        }
        token = strtok(NULL, ",");
        vector3->b = atof(token);
}

Color ColorLerp2(Color a, Color b, float t) {
    Color result;
    result.r = a.r + (b.r - a.r) * t;
    result.g = a.g + (b.g - a.g) * t;
    result.b = a.b + (b.b - a.b) * t;
    return result;
}

// needs to take arguments 
// arguments:
        //int nxn grid 
        //vector3 camera location, vector3 target, float fov_degrees
        //string output filepath 
        //optional:
                //rgb lowIntensityColor
                //rgb highIntensityColor
                //float opacity 
                //float opacity intensity ratio 
                //bool grid 




int main (int argc, char *argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 3840;
	const int screenHeight = 2560;

	InitWindow(screenWidth, screenHeight, "graph rbk g15");

	//arguments 
	int** sensorArray;   
	int width, height;
	//camera
	Vector3 cameraPosition = {0.0f, 10.0f, 10.0f };
	Vector3 cameraTarget = {0.0f, 0.0f, 0.0f };
	Vector3 cameraUp = { 0, 1, 0 };
	float cameraFOV = 45.0f;
	char filePath[256]; 

	//optional
	Color lowIntensityColor = GREEN; 
	Color highIntensityColor = RED; 
	float opacity = 1.0f;
	float opacityIntensityRatio = 1.0f;
	int drawGrid = 1;
	int graphBoxWidth = 10;

	//parse arguments
	if (argc < 8) {
	printf(
	    "Usage:\n"
	    "  %s <output_file> <2D_array> <camera_position> <camera_target> <camera_up> <fov_degrees> [low_color] [high_color] [opacity] [opacity_intensity_ratio]\n\n"
	    "Arguments:\n"
	    "  output_file                Path to save the screenshot (e.g., output.png)\n"
	    "  2D_array                   Grid in format WIDTHxHEIGHT:z,z,z,... (e.g., 20x20:1,2,3,...)\n"
	    "  camera_position            Camera position vector (e.g., 100.0,100.0,300.0)\n"
	    "  camera_target              Point the camera looks at (e.g. 150.0,150.0,0.0)\n"
	    "  camera_up                 'Up' direction vector (usually 0.0,1.0,0.0)\n"
	    "  fov_degrees                Camera field of view in degrees (e.g. 45.0)\n"
	    "Optional:\n"
	    "  low_color                  Color for lowest intensity, RGB format (e.g., 0,255,0)\n"
	    "  high_color                 Color for highest intensity, RGB format (e.g., 255,0,0)\n"
	    "  opacity                    Opacity value from 0.0 (transparent) to 1.0 (opaque)\n"
	    "  opacity_intensity_ratio    Scale opacity by intensity: 0.0-1.0 sets min percentage of previous argument for lowest intensity (e.g., 0.70)\n",
	    argv[0]
	);
	exit(EXIT_FAILURE);
	}

	strncpy(filePath, argv[1], sizeof(filePath));
	filePath[sizeof(filePath)-1] = '\0';
	parse2DArray(&width, &height, argv[2],&sensorArray);
	parseVector3(argv[3],&cameraPosition);
//	parseVector3(argv[4],&cameraTarget);
	parseVector3(argv[5],&cameraUp);
	cameraFOV = atof(argv[6]);

	printf("Camera Position: (%f, %f, %f)\n", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	printf("Camera Target:   (%f, %f, %f)\n", cameraTarget.x, cameraTarget.y, cameraTarget.z);
	printf("Camera Up:       (%f, %f, %f)\n", cameraUp.x, cameraUp.y, cameraUp.z);


	if(argc > 7) parseColor(argv[7], &lowIntensityColor);
	if(argc > 8) parseColor(argv[8], &highIntensityColor);
	if(argc > 9) {
		opacity = atof(argv[9]);
		lowIntensityColor.a = (int) (opacity * 255);
		highIntensityColor.a = (int) (opacity * 255);
	}
	if(argc > 10) opacityIntensityRatio = atof(argv[10]);

	Camera camera = { cameraPosition, cameraTarget, cameraUp, cameraFOV, 0 }; // 0 is projection

//	SetCameraMode(camera, CAMERA_FIRST_PERSON); 	

	//find max/min intensity for interpolation of color 
	int maxIntensity = 0; 
	int minIntensity = 0; 
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++){
			if(sensorArray[i][j] > maxIntensity) {
				maxIntensity = sensorArray[i][j];
			}
			if(sensorArray[i][j] < minIntensity) {
				minIntensity = sensorArray[i][j];
			}
		}
	}

	//make it be at zero zero
	//float offsetX = (width * graphBoxWidth) / 2.0f;
	//float offsetZ = (height * graphBoxWidth) / 2.0f;
	float offsetX = ((width  - 1) * graphBoxWidth) / 2.0f;
	float offsetZ = ((height - 1) * graphBoxWidth) / 2.0f;

	printf("offset X (%f)\n", offsetX);
	printf("offset Z (%f)\n", offsetZ);

		



	SetTargetFPS(60);               // set to 60 fps (not sure if needed)
	BeginDrawing();

		ClearBackground(RAYWHITE); // could add option to change background color 


		BeginMode3D(camera);

//			UpdateCamera(&camera);

			printf("Camera Target:   (%f, %f, %f)\n", camera.target.x, camera.target.y, camera.target.z);
			float minOpacity = (255.0*opacityIntensityRatio);
			float opacityRange = 255 - minOpacity;
			
			printf("height %d, width: %d", height, width);

			for(int i = 0; i < height; i++) {
				for(int j = 0; j < width; j++){
					float range = (maxIntensity - minIntensity); // otherwise would crash if min = max
					float interpolationPercentage = range == 0 ? 0.0f : (sensorArray[i][j] - minIntensity) / range;
					Color thisColor = ColorLerp(lowIntensityColor, highIntensityColor, interpolationPercentage);
					thisColor.a = (int) (minOpacity + (opacityRange*interpolationPercentage));
					Vector3 thisBoxPos = {(float)(j * graphBoxWidth)-offsetX, (float)sensorArray[i][j]/2, (float)(i*graphBoxWidth)-offsetZ};
					DrawCube(thisBoxPos, graphBoxWidth, (float)sensorArray[i][j], graphBoxWidth, thisColor);
					printf("Cube Position: (%f, %f, %f)\n", thisBoxPos.x, thisBoxPos.y, thisBoxPos.z);		
				}
			}
			if(drawGrid) {
				DrawGrid(width - 1, (float)graphBoxWidth);
			}


			float width2 = 1.0f;
			float height2 = 1.0f;
			float length2 = 1.0f;

			DrawCube(cameraTarget, width2, height2, length2, RED);

		EndMode3D();


		char buffer[300];
		time_t t = time(NULL);
		struct tm timeNow = *localtime(&t);
		sprintf(buffer, "%s_%d-%02d-%02d_%02d:%02d:%02d.png", filePath, timeNow.tm_year + 1900, timeNow.tm_mon + 1, timeNow.tm_mday, timeNow.tm_hour, timeNow.tm_min, timeNow.tm_sec);
		TakeScreenshot(buffer);
		printf("s\n", buffer);
		EndDrawing();

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
	return 0;
}
