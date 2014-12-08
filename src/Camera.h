#pragma once

//using DirectXMath library
#include <Windows.h>
#include <DirectxMath.h>
#include <assert.h>

#include "HRTimer.h"
#include "BitOperations.h"

/*
 * Class Camera
 * Freelook timer-based camera
 */

using namespace DirectX;

class Camera
{
public:
		
    enum CameraMessage
    {
        CAMERA_FORWARD_UP,
        CAMERA_FORWARD_DOWN,
        CAMERA_BACKWARD_UP,
        CAMERA_BACKWARD_DOWN,
        CAMERA_LEFT_UP,
        CAMERA_LEFT_DOWN,
        CAMERA_RIGHT_UP,
        CAMERA_RIGHT_DOWN,
        CAMERA_UP_UP,
        CAMERA_UP_DOWN,
        CAMERA_DOWN_UP,
        CAMERA_DOWN_DOWN
    };

    //Methods
    //********************************************************************
    Camera();
	//positioning and movement methods	
	void PositionCamera(const XMFLOAT3 &camPosition, const XMFLOAT3 & focusPoint, const XMFLOAT3 &upDirection);
	void HandleInputMessage( CameraMessage m );
	void AdjustOrientation(const float hRad, const float vRad);
	void SetMovementSpeed( const float s );
    void ResetButtons();

	//update method - controls camera movement
	void Update();
				
	//projection
	void SetPerspectiveProjectionLH( float FOVY, float aspectRatio, float zNear, float zFar );
		
	//get 
	XMFLOAT3   GetCameraPosition();
	XMFLOAT3   GetCameraUpVector();
	XMFLOAT3   GetCameraViewVector();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();
	XMFLOAT4X4 GetViewProjectionMatrix();

    //Members
    //********************************************************************
protected:

    //flags bitfield
    unsigned int _flags;

    //camera position and matrix data
    XMFLOAT3 _xmf3Position, _xmf3View, _xmf3Up, _xmf3Right;
    XMFLOAT4X4 _viewMatrix, _projectionMatrix, _viewProjectionMatrix;

    //free look variables
    float _yaw, _pitch, _movementSpeed;

    //camera timer
    HRTimer _timer;

    enum
    {
        CAMERA_FORWARD_BIT = 0,
        CAMERA_BACKWARD_BIT,
        CAMERA_LEFT_BIT,
        CAMERA_RIGHT_BIT,
        CAMERA_UP_BIT,
        CAMERA_DOWN_BIT
    };
};
