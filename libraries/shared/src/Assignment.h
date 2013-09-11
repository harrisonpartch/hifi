//
//  Assignment.h
//  hifi
//
//  Created by Stephen Birarda on 8/22/13.
//  Copyright (c) 2013 HighFidelity, Inc. All rights reserved.
//

#ifndef __hifi__Assignment__
#define __hifi__Assignment__

#include <sys/time.h>

#include "NodeList.h"

/// Holds information used for request, creation, and deployment of assignments
class Assignment {
public:
    
    enum Type {
        AudioMixerType,
        AvatarMixerType,
        AllTypes
    };
    
    enum Direction {
        CreateDirection,
        DeployDirection,
        RequestDirection
    };
    
    enum Location {
        GlobalLocation,
        LocalLocation
    };
    
    Assignment(Assignment::Direction direction,
               Assignment::Type type,
               Assignment::Location location = Assignment::GlobalLocation);
    
    /// Constructs an Assignment from the data in the buffer
    /// \param dataBuffer the source buffer to un-pack the assignment from
    /// \param numBytes the number of bytes left to read in the source buffer
    Assignment(const unsigned char* dataBuffer, int numBytes);
    
    ~Assignment();
    
    Assignment::Direction getDirection() const { return _direction; }
    Assignment::Type getType() const { return _type; }
    Assignment::Location getLocation() const { return _location; }
    const timeval& getTime() const { return _time; }
    
    const sockaddr* getAttachedPublicSocket() { return _attachedPublicSocket; }
    void setAttachedPublicSocket(const sockaddr* attachedPublicSocket);
    
    const sockaddr* getAttachedLocalSocket() { return _attachedLocalSocket; }
    void setAttachedLocalSocket(const sockaddr* attachedLocalSocket);
    
    /// Packs the assignment to the passed buffer
    /// \param buffer the buffer in which to pack the assignment
    /// \return number of bytes packed into buffer
    int packToBuffer(unsigned char* buffer);
    
    /// Sets _time to the current time given by gettimeofday
    void setCreateTimeToNow() { gettimeofday(&_time, NULL); }
    
private:
    Assignment::Direction _direction; /// the direction of the assignment (Create, Deploy, Request)
    Assignment::Type _type; /// the type of the assignment, defines what the assignee will do
    Assignment::Location _location; /// the location of the assignment, allows a domain to preferentially use local ACs
    sockaddr* _attachedPublicSocket; /// pointer to a public socket that relates to assignment, depends on direction
    sockaddr* _attachedLocalSocket; /// pointer to a local socket that relates to assignment, depends on direction
    timeval _time; /// time the assignment was created (set in constructor)
};

QDebug operator<<(QDebug debug, const Assignment &assignment);

#endif /* defined(__hifi__Assignment__) */