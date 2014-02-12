//
//  CCPomeloWrapper.h
//
//  Created by laoyur@126.com on 13-11-22.
//
//  A simple libpomelo wrapper for cocos2d-x
//  
//  Version history:
//  0.9 2014-01-26  1.cocos2dx_3.0 && c++11 supported
//                  2.添加中文注释，方便国内用户快速使用
//  < 0.9   2013    first released version

#ifndef __CCPomeloWrapper__
#define __CCPomeloWrapper__

#include "cocos2d.h"

#if COCOS2D_VERSION >= 0x00030000
    #define CCX3 1  //cocos2dx 3.0+
    #include <functional>
#else
    #define CCX3 0  //cocos2dx 2.x
#endif


enum CCPomeloStatus
{
    EPomeloStopped,
    EPomeloConnecting,
    EPomeloConnected
};


class CCPomeloImpl;

class CCPomeloRequestResult
{
public:
    int status;         //status code
    std::string requestRoute;
    std::string jsonMsg;
    
private:
    CCPomeloRequestResult();
    friend class CCPomeloImpl;
};

class CCPomeloNotifyResult
{
public:
    int status;
    std::string notifyRoute;
    
private:
    CCPomeloNotifyResult();
    friend class CCPomeloImpl;
};

class CCPomeloEvent
{
public:
    std::string event;
    std::string jsonMsg;
private:
    CCPomeloEvent();
    friend class CCPomeloImpl;
};

#if CCX3
    typedef std::function<void(int)> PomeloAsyncConnCallback;
    typedef std::function<void(const CCPomeloRequestResult&)> PomeloReqResultCallback;
    typedef std::function<void(const CCPomeloNotifyResult&)> PomeloNtfResultCallback;
    typedef std::function<void(const CCPomeloEvent&)> PomeloEventCallback;
#else
    typedef void (cocos2d::CCObject::*PomeloAsyncConnHandler)(int);
    typedef void (cocos2d::CCObject::*PomeloReqResultHandler)(const CCPomeloRequestResult&);
    typedef void (cocos2d::CCObject::*PomeloNtfResultHandler)(const CCPomeloNotifyResult&);
    typedef void (cocos2d::CCObject::*PomeloEventHandler)(const CCPomeloEvent&);

    #define pomelo_async_conn_cb_selector(_SEL) (PomeloAsyncConnHandler)(&_SEL)
    #define pomelo_req_result_cb_selector(_SEL) (PomeloReqResultHandler)(&_SEL)
    #define pomelo_ntf_result_cb_selector(_SEL) (PomeloNtfResultHandler)(&_SEL)
    #define pomelo_listener_cb_selector(_SEL) (PomeloEventHandler)(&_SEL)
#endif

class CCPomeloWrapper : 
#if CCX3
    public cocos2d::Object
#else
    public cocos2d::CCObject
#endif
{
public:
    static CCPomeloWrapper* getInstance();
    ~CCPomeloWrapper();
    
public:
    //get the current connection status
    //获取当前连接状态
    CCPomeloStatus status() const;
    
    //@return: 0--connect succeeded; others--connect failed
    //!!!use the async version in production!!!
    //同步连接服务器
    int connect(const char* host, int port);
    
#if CCX3
    int connectAsnyc(const char* host, int port, const PomeloAsyncConnCallback& callback);
#else
    //@return: 0--connect request succeeded; others--connect request failed
    //异步连接服务器
    int connectAsnyc(const char* host, int port, cocos2d::CCObject* pCallbackTarget, PomeloAsyncConnHandler pCallbackSelector);
#endif
    
    //stop the current connection
    //断开当前连接
    void stop();
    
#if CCX3
    void setDisconnectedCallback(const std::function<void()>& callback);
#else
    //callback when connection lost
    //监控连接丢失事件
    void setDisconnectedCallback(cocos2d::CCObject* pTarget, cocos2d::SEL_CallFunc pSelector);
#endif
    
#if CCX3
    int request(const char* route, const std::string& msg, const PomeloReqResultCallback& callback);
#else
    //send request to server
    //@return: 0--request sent succeeded; others--request sent failed
    //发送request
    int request(const char* route, const std::string& msg, cocos2d::CCObject* pCallbackTarget, PomeloReqResultHandler pCallbackSelector);
#endif
    
#if CCX3
    int notify(const char* route, const std::string& msg, const PomeloNtfResultCallback& callback);
#else
    //send notify to server
    //@return: 0--notify sent succeeded; others--notify sent failed
    //发送notify
    //可能有人会有疑问，notify不是没有reply的嘛，为毛这里还要传callback
    //这里的callback只是“notify发送成功”的callback
    int notify(const char* route, const std::string& msg, cocos2d::CCObject* pCallbackTarget, PomeloNtfResultHandler pCallbackSelector);
#endif
    
#if CCX3
    int addListener(const char* event, const PomeloEventCallback& callback);
#else
    //listen to event
    //only one listener allowed for one event currently
    //@return: 0--add listener succeeded; others--add listener failed
    //订阅事件。对于同一种事件，目前只支持一个观察者。也就是说对同一事件多次订阅，前一个订阅者自动被移除。
    int addListener(const char* event, cocos2d::CCObject* pCallbackTarget, PomeloEventHandler pCallbackSelector);
#endif
    
    //remove listener for specific event
    //移除事件订阅
    void removeListener(const char* event);
    
    //remove all listeners for all events
    //移除所有事件订阅
    void removeAllListeners();
    
    
private:
    CCPomeloWrapper();
    
private:
    CCPomeloImpl*   _theMagic;
    friend class CCPomeloImpl;
};

#endif /* defined(__CCPomeloWrapper__) */
