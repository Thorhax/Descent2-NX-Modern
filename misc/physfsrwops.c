/*
 * This code provides a glue layer between PhysicsFS and Simple Directmedia
 *  Layer's (SDL) RWops i/o abstraction.
 *
 * License: this code is public domain.
 */

#include <stdio.h>
#include "physfsrwops.h"

static Sint64 physfsrwops_size(SDL_RWops *rw)
{
    PHYSFS_file *handle = (PHYSFS_file *) rw->hidden.unknown.data1;
    return (Sint64) PHYSFS_fileLength(handle);
}

static Sint64 physfsrwops_seek(SDL_RWops *rw, Sint64 offset, int whence)
{
    PHYSFS_file *handle = (PHYSFS_file *) rw->hidden.unknown.data1;
    PHYSFS_sint64 pos = 0;

    if (whence == SEEK_SET)
    {
        pos = offset;
    }
    else if (whence == SEEK_CUR)
    {
        PHYSFS_sint64 current = PHYSFS_tell(handle);
        if (current == -1)
        {
            SDL_SetError("Can't find position in file: %s", PHYSFS_getLastError());
            return -1;
        }
        pos = current + offset;
    }
    else if (whence == SEEK_END)
    {
        PHYSFS_sint64 len = PHYSFS_fileLength(handle);
        if (len == -1)
        {
            SDL_SetError("Can't find end of file: %s", PHYSFS_getLastError());
            return -1;
        }
        pos = len + offset;
    }
    else
    {
        SDL_SetError("Invalid 'whence' parameter.");
        return -1;
    }

    if (pos < 0)
    {
        SDL_SetError("Attempt to seek past start of file.");
        return -1;
    }

    if (!PHYSFS_seek(handle, (PHYSFS_uint64) pos))
    {
        SDL_SetError("PhysicsFS error: %s", PHYSFS_getLastError());
        return -1;
    }

    return pos;
}

static size_t physfsrwops_read(SDL_RWops *rw, void *ptr, size_t size, size_t maxnum)
{
    PHYSFS_file *handle = (PHYSFS_file *) rw->hidden.unknown.data1;
    PHYSFS_sint64 rc = PHYSFS_read(handle, ptr, size, maxnum);
    if (rc < 0)
        rc = 0;
    return (size_t) rc;
}

static size_t physfsrwops_write(SDL_RWops *rw, const void *ptr, size_t size, size_t num)
{
    PHYSFS_file *handle = (PHYSFS_file *) rw->hidden.unknown.data1;
    PHYSFS_sint64 rc = PHYSFS_write(handle, ptr, size, num);
    if (rc < 0)
        rc = 0;
    return (size_t) rc;
}

static int physfsrwops_close(SDL_RWops *rw)
{
    PHYSFS_file *handle = (PHYSFS_file *) rw->hidden.unknown.data1;
    if (handle)
        PHYSFS_close(handle);
    SDL_FreeRW(rw);
    return 0;
}

static SDL_RWops *create_rwops(PHYSFS_file *handle)
{
    SDL_RWops *retval = NULL;

    if (handle == NULL)
        SDL_SetError("PhysicsFS error: %s", PHYSFS_getLastError());
    else
    {
        retval = SDL_AllocRW();
        if (retval != NULL)
        {
            retval->size  = physfsrwops_size;
            retval->seek  = physfsrwops_seek;
            retval->read  = physfsrwops_read;
            retval->write = physfsrwops_write;
            retval->close = physfsrwops_close;
            retval->type  = SDL_RWOPS_UNKNOWN;
            retval->hidden.unknown.data1 = handle;
        }
    }

    return retval;
}

SDL_RWops *PHYSFSRWOPS_makeRWops(PHYSFS_file *handle)
{
    SDL_RWops *retval = NULL;
    if (handle == NULL)
        SDL_SetError("NULL pointer pass to PHYSFSRWOPS_makeRWops().");
    else
        retval = create_rwops(handle);
    return retval;
}

SDL_RWops *PHYSFSRWOPS_openRead(const char *fname)
{
    return create_rwops(PHYSFS_openRead(fname));
}

SDL_RWops *PHYSFSRWOPS_openWrite(const char *fname)
{
    return create_rwops(PHYSFS_openWrite(fname));
}

SDL_RWops *PHYSFSRWOPS_openAppend(const char *fname)
{
    return create_rwops(PHYSFS_openAppend(fname));
}
