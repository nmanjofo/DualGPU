#include "Camera.h"

#include <iostream>


//constructor
Camera::Camera():	_flags(0),
					_xmf3Position( XMFLOAT3(0,0,0) ),
					_xmf3View( XMFLOAT3(0,0,1) ),
					_xmf3Up( XMFLOAT3(0,1,0) ),
					_xmf3Right(XMFLOAT3(1,0,0)),
					_yaw(0), 
					_pitch(0), 
					_movementSpeed(0) 
{
	//set all matrices to identity matrix
	XMMATRIX m = XMMatrixIdentity();
	XMStoreFloat4x4(&_viewMatrix, m);
	XMStoreFloat4x4(&_projectionMatrix, m);
	XMStoreFloat4x4(&_viewProjectionMatrix, m);	

	//start camera _timer
	_timer.Start();
}

void Camera::PositionCamera(const XMFLOAT3 &camPosition, const XMFLOAT3 &focusPoint, const XMFLOAT3 &upDirection)
{
    assert(!XMVector3Equal(XMLoadFloat3(&focusPoint), XMVectorZero()));
    assert(!XMVector3Equal(XMLoadFloat3(&upDirection), XMVectorZero()));

	//create view matrix
	XMVECTOR cp = XMLoadFloat3(&camPosition);
	XMVECTOR fp = XMLoadFloat3(&focusPoint);
	XMVECTOR ud = XMLoadFloat3(&upDirection);

	//--
	fp = XMVector3Normalize(XMVectorSubtract(fp, cp));
	ud = XMVector3Normalize(ud);
	//--

	XMMATRIX vm = XMMatrixLookAtLH( cp, fp, ud);
	XMStoreFloat4x4( &_viewMatrix, vm);
	
	//set cam vars
	_xmf3Position = camPosition;
	XMStoreFloat3( &_xmf3View, fp);
	XMStoreFloat3( &_xmf3Up, ud);
	XMStoreFloat3( &_xmf3Right, XMVector3Cross(ud,fp-cp));
}
//projection methods
void Camera::SetPerspectiveProjectionLH(float FOVY, float aspectRatio, float zNear, float zFar)
{
	//create projection matrix and store it in a XMFLOAT4X4
    XMMATRIX p2 = XMMatrixPerspectiveFovLH(FOVY, aspectRatio, zNear, zFar);
	XMStoreFloat4x4( &_projectionMatrix, p2);	
}
//get methods
XMFLOAT3 Camera::GetCameraPosition() 
{ 
	return _xmf3Position; 
}
 XMFLOAT3 Camera::GetCameraUpVector()
{	
	return _xmf3Up;
}
 XMFLOAT3 Camera::GetCameraViewVector()
{
	return _xmf3View;
}
 XMFLOAT4X4 Camera::GetViewMatrix() 
{ 
	return _viewMatrix; 
}
 XMFLOAT4X4 Camera::GetProjectionMatrix() 
{ 
	return _projectionMatrix; 
}
 XMFLOAT4X4 Camera::GetViewProjectionMatrix()
{
	return _viewProjectionMatrix;
}

//handle input message - WSAD, button state down/up
void Camera::HandleInputMessage( CameraMessage msg )
{
    switch (msg)
    {
    case CAMERA_FORWARD_DOWN:
        SetBit(_flags, CAMERA_FORWARD_BIT);
        break;
    case CAMERA_FORWARD_UP:
        ClearBit(_flags, CAMERA_FORWARD_BIT);
        break;

    case CAMERA_BACKWARD_DOWN:
        SetBit(_flags, CAMERA_BACKWARD_BIT);
        break;
    case CAMERA_BACKWARD_UP:
        ClearBit(_flags, CAMERA_BACKWARD_BIT);
        break;

    case CAMERA_LEFT_DOWN:
        SetBit(_flags, CAMERA_LEFT_BIT);
        break;
    case CAMERA_LEFT_UP:
        ClearBit(_flags, CAMERA_LEFT_BIT);
        break;

    case CAMERA_RIGHT_DOWN:
        SetBit(_flags, CAMERA_RIGHT_BIT);
        break;
    case CAMERA_RIGHT_UP:
        ClearBit(_flags, CAMERA_RIGHT_BIT);
        break;

    case CAMERA_UP_DOWN:
        SetBit(_flags, CAMERA_UP_BIT);
        break;
    case CAMERA_UP_UP:
        ClearBit(_flags, CAMERA_UP_BIT);
        break;

    case CAMERA_DOWN_DOWN:
        SetBit(_flags, CAMERA_DOWN_BIT);
        break;
    case CAMERA_DOWN_UP:
        ClearBit(_flags, CAMERA_DOWN_BIT);
        break;

    default:
        break;
    }
}

//update camera view
void Camera::Update()
{
	//get current elapsed time
	float t = (float) _timer.GetElapsedTimeSeconds();
	
    XMMATRIX m;
    if (_pitch != 0 || _yaw != 0)
    {
        //create rotation matrix using quaternions
        XMVECTOR qYRot = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _yaw);
        XMVECTOR qRightRot = XMQuaternionRotationAxis(XMLoadFloat3(&_xmf3Right), _pitch);
		XMVECTOR qRot = XMQuaternionMultiply(qRightRot, qYRot);

        m = XMMatrixRotationQuaternion(qRot);
   }
   else
   {
       m = XMMatrixIdentity();
   }

	//create new view, up and right vectors	
	XMVECTOR vView, vUp, vRight;
	vUp = XMLoadFloat3(&_xmf3Up);
	vView = XMLoadFloat3(&_xmf3View);

	vView = XMVector3Normalize(XMVector3Transform(vView, m));
	vUp = XMVector3Normalize(XMVector3Transform(vUp, m));	
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vView));
	
    XMVECTOR vYAxis = XMVECTOR(XMLoadFloat3(&XMFLOAT3(0, 1, 0)));
	
	//update camera position
	XMVECTOR vPos = XMLoadFloat3(&_xmf3Position);
	
	if (IsBitSet(_flags, CAMERA_FORWARD_BIT) ) 
        vPos += _movementSpeed * t * vView;	//forward
    if (IsBitSet(_flags, CAMERA_BACKWARD_BIT)) 
        vPos -= _movementSpeed * t * vView;	//back
    if (IsBitSet(_flags, CAMERA_LEFT_BIT))     
        vPos -= _movementSpeed * t * vRight; //left
    if (IsBitSet(_flags, CAMERA_RIGHT_BIT))    
        vPos += _movementSpeed * t * vRight; //right
    if (IsBitSet(_flags, CAMERA_UP_BIT))       
        vPos += _movementSpeed * t * vYAxis; //up
    if (IsBitSet(_flags, CAMERA_DOWN_BIT))     
        vPos -= _movementSpeed * t * vYAxis; //down
	
	//create view matrix
	m = XMMatrixLookToLH(vPos, vView, vUp);
	XMStoreFloat4x4(&_viewMatrix,m);
	
	//set cam vars
	XMStoreFloat3(&_xmf3Position, vPos);
	XMStoreFloat3(&_xmf3View, vView);
	XMStoreFloat3(&_xmf3Up, vUp);
	XMStoreFloat3(&_xmf3Right, vRight);

	//update view projection matrix
	XMMATRIX p = XMLoadFloat4x4(&_projectionMatrix);
	m = XMMatrixMultiply(m, p);
	XMStoreFloat4x4(&_viewProjectionMatrix, m);
	
	//reset angles and _timer
	_pitch = 0.0f;
	_yaw = 0.0f;
	_timer.Reset();
}
//adjust yaw and pitch
void Camera::AdjustOrientation( const float yRad, const float pRad )
{
	_yaw += yRad;
	_pitch += pRad;

	//value clamping - keep heading and pitch between 0 and 2 pi
	if ( _yaw > XM_2PI ) 
		_yaw -= (float) XM_2PI;
	else if ( _yaw < 0 ) 
		_yaw = (float) XM_2PI + _yaw;
	
	if ( _pitch > XM_2PI ) 
		_pitch -= (float) XM_2PI;
	else if ( _pitch < 0 ) 
		_pitch = (float) XM_2PI + _pitch;
}
//set movement speed
void Camera::SetMovementSpeed( const float s)
{
	_movementSpeed = s;
}

void Camera::ResetButtons()
{
    _flags = 0;
}

