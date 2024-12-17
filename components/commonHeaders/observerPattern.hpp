#ifndef __OBSERVER_PATTERN__
#define __OBSERVER_PATTERN__

class Observer
{
public:
    //! \brief  Updates port configuration using the data in the received buffer
    //! \param  buffer pointer to a char array
    //! \return None
    virtual void updatePortConfig(const char *buffer) = 0;
};

class Subject
{
    Observer *observer;

public:
    void registerObserver(Observer *newObserver)
    {
        observer = newObserver;
    }

    void notifyObserver(const char *buffer)
    {
        observer->updatePortConfig(buffer);
    }
};

#endif