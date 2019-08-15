#ifndef ASYNCOPERATION_H
#define ASYNCOPERATION_H

#include <QObject>
#include <QVariant>
#include <QString>
#include <QVariantMap>
#include <QPair>
#include <QMultiMap>
class Hitachi161;
enum OPERATIONS{
    Clear = 0,
    Set,
    Add,
    Remove,
    Read,
    Write
};
typedef void (*StartMethod)();
typedef  QMap<OPERATIONS,QVariantMap> ValueMap;
class AsyncOperation : public QObject
{
    Q_OBJECT
public:
    AsyncOperation(QObject* parent = nullptr);
    const AsyncOperation* await(OPERATIONS op, QString name = QString(), QVariant value = QVariant());
    void notify(OPERATIONS op, QString name = QString(), QVariant value = QVariant());
    QVariant read(QString name, OPERATIONS op = OPERATIONS::Read);
    void reset();
    bool isActive();
    bool isFinished();
    void finish();
    void start();
    void setStartParams(QVariantMap map);
    void onStart(std::function<void(void)> start_method);
    bool onFinished(const QObject *receiver, const char * method);
    bool hasValue(QString name, OPERATIONS op = OPERATIONS::Read);
    ~AsyncOperation();
signals:
    void finished();
protected:

    std::function<void()> mStartMethod;
    ValueMap  mPendingValues;
    ValueMap  mValues;
    bool      mDone;
};

#endif // ASYNCOPERATION_H
