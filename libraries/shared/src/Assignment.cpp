//
//  Assignment.cpp
//  hifi
//
//  Created by Stephen Birarda on 8/22/13.
//  Copyright (c) 2013 HighFidelity, Inc. All rights reserved.
//

#include "PacketHeaders.h"

#include "Assignment.h"

const char IPv4_ADDRESS_DESIGNATOR = 4;
const char IPv6_ADDRESS_DESIGNATOR = 6;

Assignment::Assignment(Assignment::Direction direction, Assignment::Type type, Assignment::Location location) :
    _direction(direction),
    _type(type),
    _location(location),
    _attachedPublicSocket(NULL),
    _attachedLocalSocket(NULL)
{
    // set the create time on this assignment
    gettimeofday(&_time, NULL);
}

Assignment::Assignment(const unsigned char* dataBuffer, int numBytes) :
    _location(GlobalLocation),
    _attachedPublicSocket(NULL),
    _attachedLocalSocket(NULL)
{
    // set the create time on this assignment
    gettimeofday(&_time, NULL);
    
    int numBytesRead = 0;
    
    if (dataBuffer[0] == PACKET_TYPE_REQUEST_ASSIGNMENT) {
        _direction = Assignment::RequestDirection;
    } else if (dataBuffer[0] == PACKET_TYPE_CREATE_ASSIGNMENT) {
        _direction = Assignment::CreateDirection;
    } else if (dataBuffer[0] == PACKET_TYPE_DEPLOY_ASSIGNMENT) {
        _direction = Assignment::DeployDirection;
    }
    
    numBytesRead += numBytesForPacketHeader(dataBuffer);
    
    memcpy(&_type, dataBuffer + numBytesRead, sizeof(Assignment::Type));
    numBytesRead += sizeof(Assignment::Type);
    
    if (numBytes > numBytesRead) {
        
        sockaddr* newSocket = NULL;
        
        if (dataBuffer[numBytesRead++] == IPv4_ADDRESS_DESIGNATOR) {
            // IPv4 address
            newSocket = (sockaddr*) new sockaddr_in;
            unpackSocket(dataBuffer + numBytesRead, newSocket);
        } else {
            // IPv6 address, or bad designator
            qDebug("Received a socket that cannot be unpacked!\n");
        }
        
        if (_direction == Assignment::CreateDirection) {
            delete _attachedLocalSocket;
            _attachedLocalSocket = newSocket;
        } else {
            delete _attachedPublicSocket;
            _attachedPublicSocket = newSocket;
        }
    }
}

Assignment::~Assignment() {
    delete _attachedPublicSocket;
    delete _attachedLocalSocket;
}

void Assignment::setAttachedPublicSocket(const sockaddr* attachedPublicSocket) {
    if (_attachedPublicSocket) {
        // delete the old socket if it exists
        delete _attachedPublicSocket;
        _attachedPublicSocket = NULL;
    }
    
    if (attachedPublicSocket) {
        copySocketToEmptySocketPointer(&_attachedPublicSocket, attachedPublicSocket);
    }
}

void Assignment::setAttachedLocalSocket(const sockaddr* attachedLocalSocket) {
    if (_attachedLocalSocket) {
        // delete the old socket if it exists
        delete _attachedLocalSocket;
        _attachedLocalSocket = NULL;
    }
    
    if (attachedLocalSocket) {
        copySocketToEmptySocketPointer(&_attachedLocalSocket, attachedLocalSocket);
    }
}

int Assignment::packToBuffer(unsigned char* buffer) {
    int numPackedBytes = 0;
    
    memcpy(buffer + numPackedBytes, &_type, sizeof(_type));
    numPackedBytes += sizeof(_type);
    
    if (_attachedPublicSocket || _attachedLocalSocket) {
        sockaddr* socketToPack = (_attachedPublicSocket) ? _attachedPublicSocket : _attachedLocalSocket;
        
        // we have a socket to pack, add the designator
        buffer[numPackedBytes++] = (socketToPack->sa_family == AF_INET)
            ? IPv4_ADDRESS_DESIGNATOR : IPv6_ADDRESS_DESIGNATOR;
        
        numPackedBytes += packSocket(buffer + numPackedBytes, socketToPack);
    }
    
    return numPackedBytes;
}

QDebug operator<<(QDebug debug, const Assignment &assignment) {
    debug << "T:" << assignment.getType();
    return debug.nospace();
}