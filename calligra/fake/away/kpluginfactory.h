#ifndef FAKE_KLUGINFACTORY_H
#define FAKE_KLUGINFACTORY_H

#include <QObject>
#include <QPluginLoader>
#include <QtPlugin>
#include <QJsonObject>
#include <QDebug>
#include <kcomponentdata.h>
#include <kpluginfactory.h>
//#include <kexportplugin.h>
#include <kglobal.h>
#include <klibloader.h>

namespace KParts { class Part; }

#define K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations) \
    class name : public baseFactory { \
    public: \
        explicit name(const char *componentName = 0, const char *catalogName = 0, QObject *parent = 0) : baseFactory(componentName, catalogName, parent) { init(); } \
        explicit name(const KAboutData &aboutData, QObject *parent = 0) : baseFactory(aboutData, parent) { init(); } \
        static KComponentData componentData() { return KComponentData(); } \
    private: \
        void init() { pluginRegistrations } \
    };

#define K_PLUGIN_FACTORY(name, pluginRegistrations) K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)

#define K_EXPORT_PLUGIN(x) \
    namespace { \
        class RegisterPluginFactory { \
        public: \
            static QObject* createPluginInstance() { \
                QObject *factory = new x ; \
                return factory; \
            } \
            RegisterPluginFactory() { \
                qRegisterStaticPluginInstanceFunction( (QtPluginInstanceFunction) createPluginInstance ); \
            } \
        }; \
        RegisterPluginFactory registerPluginFactory; \
    }

#define K_PLUGIN_FACTORY_WITH_JSON(factory, jsOnName, pluginRegistrations) \
    K_PLUGIN_FACTORY(factory, pluginRegistrations)

#if 0
#define K_PLUGIN_FACTORY_DECLARATION(name) K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, KPluginFactory)
#define K_PLUGIN_FACTORY_DEFINITION(name, pluginRegistrations) K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)
#endif

class KPluginFactoryContainer : public QObject
{
    Q_OBJECT
public:
    KPluginFactoryContainer(QObject *parent = 0) : QObject(parent) { setObjectName("KPluginFactoryContainer"); }
};

class KPluginFactory : public KLibFactory
{
    Q_OBJECT
public:
    KPluginFactory(const char *componentName = 0, const char *catalogName = 0, QObject *parent = 0);
    KPluginFactory(const QByteArray &componentName, QObject *parent = 0);
    KPluginFactory(const KAboutData &aboutData, QObject *parent = 0);

    KComponentData componentData() const;

    template<typename T>
    T *create(QObject *parent = 0, const QVariantList &args = QVariantList());

    template<typename T>
    T *create(const QString &keyword, QObject *parent = 0, const QVariantList &args = QVariantList());

    template<typename T>
    T *create(QWidget *parentWidget, QObject *parent, const QString &keyword = QString(), const QVariantList &args = QVariantList());

Q_SIGNALS:
    void objectCreated(QObject *object);

protected:
    typedef QObject *(*CreateInstanceFunction)(QWidget *, QObject *, const QVariantList &);

private:
    class PluginIface {
    public:
        QString m_keyword;
        CreateInstanceFunction m_instanceFunction;
        PluginIface(const QString &keyword = QString(), CreateInstanceFunction instanceFunction = 0) : m_instanceFunction(instanceFunction) {}
        virtual QObject* create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword) = 0;
    };

    template<class T>
    class PluginImpl : public PluginIface {
    public:
        PluginImpl(const QString &keyword = QString(), CreateInstanceFunction instanceFunction = 0)
            : PluginIface(keyword, instanceFunction) {}
        virtual QObject* create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword) {
            if (m_instanceFunction)
                return m_instanceFunction(parentWidget, parent, args);
            return new T(parent, args);
        }
    };

    QHash<QString, PluginIface*> m_registeredPlugins;

protected:
    template<class T>
    void registerPlugin(const QString &keyword = QString(), CreateInstanceFunction instanceFunction = 0 ) {
        QString name = T::staticMetaObject.className();
        PluginIface *p = new PluginImpl<T>(keyword, instanceFunction);
        m_registeredPlugins.insert(name, p);
    }

    void setComponentData(const KComponentData &componentData);

    virtual void setupTranslations();

    virtual QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword);

#if 0
    template<class impl, class ParentType>
    static QObject *createInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        Q_UNUSED(parentWidget);
        ParentType *p = 0;
        if (parent) {
            p = qobject_cast<ParentType *>(parent);
            Q_ASSERT(p);
        }
        return new impl(p, args);
    }

    template<class impl>
    static QObject *createPartInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        return new impl(parentWidget, parent, args);
    }
#endif

private:
    KComponentData m_componentData;
};

Q_DECLARE_INTERFACE(KPluginFactory, "org.kde.calligra.fake.kpluginfactory")

template<typename T>
inline T *KPluginFactory::create(QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent): 0, parent, args, QString());
    T *t = qobject_cast<T*>(o);
    //return t ? t : o;
    return t;
}

template<typename T>
inline T *KPluginFactory::create(const QString &keyword, QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent): 0, parent, args, keyword);
    T *t = qobject_cast<T*>(o);
    //return t ? t : o;
    return t;
}

template<typename T>
inline T *KPluginFactory::create(QWidget *parentWidget, QObject *parent, const QString &keyword, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parentWidget, parent, args, keyword);
    T *t = qobject_cast<T*>(o);
    //return t ? t : o;
    return t;
}

#endif
 
