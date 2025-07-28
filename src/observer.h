#ifndef OBSERVER_H
#define OBSERVER_H

class Subject;

class Observer {
  public:
    // Pass the Subject that called the notify method.
    virtual void notify(Subject &whoFrom) = 0; 
    virtual ~Observer() = default;
};

#endif
