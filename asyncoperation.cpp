#include "asyncoperation.h"
AsyncOperation::AsyncOperation(QObject *parent) : QObject(parent), mStartMethod(nullptr), mDone(false)
{

}


void AsyncOperation::notify(OPERATIONS op, QString name, QVariant value)
{
    if(mPendingValues.contains(op)){
        auto values = mPendingValues.value(op);
        if(values.contains(name)){ // found a pending operation
            bool remove(false);
            if(op == OPERATIONS::Read) // if its a read,
            {
                remove = true; //remove the pending operation and add the value to mValues
                if(mValues.contains(op)){
                    auto values = mValues.value(op);
                    values.insert(name,value);
                    mValues.insert(op,values);
                }else{
                    QVariantMap map;
                    map.insert(name,value);
                    mValues.insert(op,map);
                }
            }else if(op == OPERATIONS::Write){
                auto v = values.value(name);
                if(v == value){ // matches expected value
                    remove = true;
                    if(mValues.contains(op)){
                        auto values = mValues.value(op);
                        values.insert(name,value);
                        mValues.insert(op,values);
                    }else{
                        QVariantMap map;
                        map.insert(name,value);
                        mValues.insert(op,map);
                    }
                }
            }else{
                remove = true;
            }
            if(remove){
                values.remove(name);
                if(values.size() == 0){
                    mPendingValues.remove(op);
                }else{
                    mPendingValues.insert(op,values);
                }
            }
        }
    }
    if(mPendingValues.size() == 0){
        mDone = true;
        emit finished();
    }
}

void AsyncOperation::finish()
{
    mDone = true;
    emit finished();
}

void AsyncOperation::start()
{
    if(mStartMethod){
        mStartMethod();
    }
}


void AsyncOperation::onStart(std::function<void(void)> start_method)
{
    mStartMethod = start_method;
}

const AsyncOperation* AsyncOperation::await(OPERATIONS op, QString name, QVariant value)
{
    if(mPendingValues.contains(op)){
        auto values = mPendingValues.value(op);
        values.insert(name, value);
        mPendingValues.insert(op,values);
    }else{
        QVariantMap map;
        map.insert(name, value);
        mPendingValues.insert(op,map);
    }
    return this;
}

bool AsyncOperation::onFinished(const QObject *receiver, const char* method)
{
    disconnect(this, &AsyncOperation::finished,nullptr,nullptr);
    return connect(this, SIGNAL(finished()),receiver,method);
}

QVariant AsyncOperation::read(QString name, OPERATIONS op)
{
    QVariant ret_val;
    if(mValues.contains(op)){
        auto values = mValues.value(op);
        if(values.contains(name)){
            ret_val = values.take(name);
            mValues.insert(op,values);
        }
    }
    return ret_val;
}

bool AsyncOperation::hasValue(QString name, OPERATIONS op)
{
    bool has_item(false);
    if(mValues.contains(op)){
        auto values = mValues.value(op);
        if(values.contains(name)){
            has_item = true;
        }
    }
    return has_item;
}

void AsyncOperation::reset()
{
    mPendingValues.clear();
    mValues.clear();
    mDone = false;
}

bool AsyncOperation::isActive()
{
    return (mPendingValues.size() > 0);
}

bool AsyncOperation::isFinished()
{
    return mDone;
}

AsyncOperation::~AsyncOperation()
{
    reset();
}
