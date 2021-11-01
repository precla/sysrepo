/**
 * @file sysrepo_types.h
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief public sysrepo types header
 *
 * @copyright
 * Copyright (c) 2018 - 2021 Deutsche Telekom AG.
 * Copyright (c) 2018 - 2021 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#ifndef _SYSREPO_TYPES_H
#define _SYSREPO_TYPES_H

#include <inttypes.h>
#include <stddef.h>
#include <sys/stat.h>

struct lyd_node;
struct timespec;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup log_api
 * @{
 */

/**
 * @brief Sysrepo error codes.
 */
typedef enum {
    SR_ERR_OK = 0,             /**< No error. */
    SR_ERR_INVAL_ARG,          /**< Invalid argument. */
    SR_ERR_LY,                 /**< Error generated by libyang. */
    SR_ERR_SYS,                /**< System function call failed. */
    SR_ERR_NO_MEMORY,          /**< Not enough memory. */
    SR_ERR_NOT_FOUND,          /**< Item not found. */
    SR_ERR_EXISTS,             /**< Item already exists. */
    SR_ERR_INTERNAL,           /**< Other internal error. */
    SR_ERR_UNSUPPORTED,        /**< Unsupported operation requested. */
    SR_ERR_VALIDATION_FAILED,  /**< Validation of the changes failed. */
    SR_ERR_OPERATION_FAILED,   /**< An operation failed. */
    SR_ERR_UNAUTHORIZED,       /**< Operation not authorized. */
    SR_ERR_LOCKED,             /**< Requested resource is already locked. */
    SR_ERR_TIME_OUT,           /**< Time out has expired. */
    SR_ERR_CALLBACK_FAILED,    /**< User callback failure caused the operation to fail. */
    SR_ERR_CALLBACK_SHELVE     /**< User callback has not processed the event and will do so
                                    on some future event processing. */
} sr_error_t;

/**
 * @brief Log levels used to determine if message of certain severity should be printed.
 */
typedef enum {
    SR_LL_NONE = 0,  /**< Do not print any messages. */
    SR_LL_ERR,       /**< Print only error messages. */
    SR_LL_WRN,       /**< Print error and warning messages. */
    SR_LL_INF,       /**< Besides errors and warnings, print some other informational messages. */
    SR_LL_DBG        /**< Print all messages including some development debug messages. */
} sr_log_level_t;

/**
 * @brief Sets callback that will be called when a log entry would be populated.
 *
 * @param[in] level Verbosity level of the log entry.
 * @param[in] message Message of the log entry.
 */
typedef void (*sr_log_cb)(sr_log_level_t level, const char *message);

/** @} */

/**
 * @ingroup conn_sess_api
 * @{
 */

/**
 * @brief Sysrepo connection.
 */
typedef struct sr_conn_ctx_s sr_conn_ctx_t;

/**
 * @brief Sysrepo session on a connection.
 */
typedef struct sr_session_ctx_s sr_session_ctx_t;

/**
 * @brief Flags used to override default connection handling by ::sr_connect call.
 */
typedef enum {
    SR_CONN_DEFAULT = 0,            /**< No special behaviour. */
    SR_CONN_CACHE_RUNNING = 1       /**< Always cache running datastore data which makes mainly repeated retrieval of data
                                         much faster. Affects all sessions created on this connection. */
} sr_conn_flag_t;

/**
 * @brief Options overriding default connection handling by ::sr_connect call,
 * it is supposed to be bitwise OR-ed value of any ::sr_conn_flag_t flags.
 */
typedef uint32_t sr_conn_options_t;

/**
 * @brief [Datastores](@ref datastores) that sysrepo supports. To change which datastore a session operates on,
 * use ::sr_session_switch_ds.
 */
typedef enum {
    SR_DS_STARTUP = 0,     /**< Contains configuration data that will be loaded when a device starts. */
    SR_DS_RUNNING = 1,     /**< Contains current configuration data. */
    SR_DS_CANDIDATE = 2,   /**< Contains prepared configuration data that do not affect actual configuration. */
    SR_DS_OPERATIONAL = 3  /**< Contains currently used configuration (see [operational datastore](@ref oper_ds)). */
} sr_datastore_t;

/**
 * @brief Count of all the datastore types
 */
#define SR_DS_COUNT 4

/**
 * @brief Special notification datastore of a module.
 */
#define SR_MOD_DS_NOTIF 4

/**
 * @brief Count of all module datastore plugin types (datastores and notifications).
 */
#define SR_MOD_DS_PLUGIN_COUNT 5

/**
 * @brief Custom datastore implementation config for each datastore and notifications of a module.
 */
struct sr_module_ds_s {
    const char *plugin_name[SR_MOD_DS_PLUGIN_COUNT];    /**< Datastore plugin name. */
};

typedef struct sr_module_ds_s sr_module_ds_t;

/**
 * @brief A single, detailed error message. Used in sr_error_info_t
 */
struct sr_error_info_err_s {
    sr_error_t err_code;    /**< Error code. */
    char *message;          /**< Error message. */
    char *error_format;     /**< Error format identifier. */
    void *error_data;       /**< Opaque error data specific for @p error_format. */
};

typedef struct sr_error_info_err_s sr_error_info_err_t;

/**
 * @brief Detailed sysrepo session error information.
 */
struct sr_error_info_s {
    sr_error_info_err_t *err;   /**< Array of all generated errors. */
    uint32_t err_count;         /**< Error count. */
};

typedef struct sr_error_info_s sr_error_info_t;

/**
 * @brief Callback to be called before applying a diff. Set it using ::sr_set_diff_check_callback.
 *
 * @param[in] session Implicit session (do not stop) with information about the event originator session IDs.
 * @param[in] diff Diff to be applied.
 * @return Error code (::SR_ERR_OK on success).
 */
typedef int (*sr_diff_check_cb)(sr_session_ctx_t *session, const struct lyd_node *diff);

/** @} connsess */

/**
 * @ingroup get_data_api
 * @{
 */

/**
 * @brief Structure that safely wraps libyang data and prevents unexpected context changes.
 */
struct sr_data_s {
    /** Connection whose context was used for creating @p tree. */
    const sr_conn_ctx_t *conn;

    /** Arbitrary libyang data, it can be modified */
    struct lyd_node *tree;
};

typedef struct sr_data_s sr_data_t;

/**
 * @brief Possible types of a data element stored in the sysrepo datastore.
 */
typedef enum {
    /* special types that does not contain any data */
    SR_UNKNOWN_T,              /**< Element unknown to sysrepo (unsupported element). */

    SR_LIST_T,                 /**< List instance. ([RFC 7950 sec 7.8](http://tools.ietf.org/html/rfc7950#section-7.8)) */
    SR_CONTAINER_T,            /**< Non-presence container. ([RFC 7950 sec 7.5](http://tools.ietf.org/html/rfc7950#section-7.5)) */
    SR_CONTAINER_PRESENCE_T,   /**< Presence container. ([RFC 7950 sec 7.5.1](http://tools.ietf.org/html/rfc7950#section-7.5.1)) */
    SR_LEAF_EMPTY_T,           /**< A leaf that does not hold any value ([RFC 7950 sec 9.11](http://tools.ietf.org/html/rfc7950#section-9.11)) */
    SR_NOTIFICATION_T,         /**< Notification instance ([RFC 7095 sec 7.16](https://tools.ietf.org/html/rfc7950#section-7.16)) */

    /* types containing some data */
    SR_BINARY_T,       /**< Base64-encoded binary data ([RFC 7950 sec 9.8](http://tools.ietf.org/html/rfc7950#section-9.8)) */
    SR_BITS_T,         /**< A set of bits or flags ([RFC 7950 sec 9.7](http://tools.ietf.org/html/rfc7950#section-9.7)) */
    SR_BOOL_T,         /**< A boolean value ([RFC 7950 sec 9.5](http://tools.ietf.org/html/rfc7950#section-9.5)) */
    SR_DECIMAL64_T,    /**< 64-bit signed decimal number ([RFC 7950 sec 9.3](http://tools.ietf.org/html/rfc7950#section-9.3)) */
    SR_ENUM_T,         /**< A string from enumerated strings list ([RFC 7950 sec 9.6](http://tools.ietf.org/html/rfc7950#section-9.6)) */
    SR_IDENTITYREF_T,  /**< A reference to an abstract identity ([RFC 7950 sec 9.10](http://tools.ietf.org/html/rfc7950#section-9.10)) */
    SR_INSTANCEID_T,   /**< References a data tree node ([RFC 7950 sec 9.13](http://tools.ietf.org/html/rfc7950#section-9.13)) */
    SR_INT8_T,         /**< 8-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_INT16_T,        /**< 16-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_INT32_T,        /**< 32-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_INT64_T,        /**< 64-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_STRING_T,       /**< Human-readable string ([RFC 7950 sec 9.4](http://tools.ietf.org/html/rfc7950#section-9.4)) */
    SR_UINT8_T,        /**< 8-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_UINT16_T,       /**< 16-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_UINT32_T,       /**< 32-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_UINT64_T,       /**< 64-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    SR_ANYXML_T,       /**< Unknown chunk of XML ([RFC 7950 sec 7.10](https://tools.ietf.org/html/rfc7950#section-7.10)) */
    SR_ANYDATA_T       /**< Unknown set of nodes, encoded in XML ([RFC 7950 sec 7.10](https://tools.ietf.org/html/rfc7950#section-7.10)) */
} sr_val_type_t;

/**
 * @brief Data of an element (if applicable), properly set according to the type.
 */
union sr_val_data_u {
    char *binary_val;       /**< Base64-encoded binary data ([RFC 7950 sec 9.8](http://tools.ietf.org/html/rfc7950#section-9.8)) */
    char *bits_val;         /**< A set of bits or flags ([RFC 7950 sec 9.7](http://tools.ietf.org/html/rfc7950#section-9.7)) */
    int bool_val;           /**< A boolean value ([RFC 7950 sec 9.5](http://tools.ietf.org/html/rfc7950#section-9.5)) */
    double decimal64_val;   /**< 64-bit signed decimal number ([RFC 7950 sec 9.3](http://tools.ietf.org/html/rfc7950#section-9.3))
                                 __Be careful with this value!__ It is not always possible and the value can change when converting
                                 between a double and YANG decimal64. Because of that you may see some unexpected behavior setting
                                 or reading this value. To avoid these problems, use `*_tree()` API variants instead. */
    char *enum_val;         /**< A string from enumerated strings list ([RFC 7950 sec 9.6](http://tools.ietf.org/html/rfc7950#section-9.6)) */
    char *identityref_val;  /**< A reference to an abstract identity ([RFC 7950 sec 9.10](http://tools.ietf.org/html/rfc7950#section-9.10)) */
    char *instanceid_val;   /**< References a data tree node ([RFC 7950 sec 9.13](http://tools.ietf.org/html/rfc7950#section-9.13)) */
    int8_t int8_val;        /**< 8-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    int16_t int16_val;      /**< 16-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    int32_t int32_val;      /**< 32-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    int64_t int64_val;      /**< 64-bit signed integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    char *string_val;       /**< Human-readable string ([RFC 7950 sec 9.4](http://tools.ietf.org/html/rfc7950#section-9.4)) */
    uint8_t uint8_val;      /**< 8-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    uint16_t uint16_val;    /**< 16-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    uint32_t uint32_val;    /**< 32-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    uint64_t uint64_val;    /**< 64-bit unsigned integer ([RFC 7950 sec 9.2](https://tools.ietf.org/html/rfc7950#section-9.2)) */
    char *anyxml_val;       /**< Unknown chunk of XML ([RFC 7950 sec 7.10](https://tools.ietf.org/html/rfc7950#section-7.10)) */
    char *anydata_val;      /**< Unknown set of nodes, encoded in XML ([RFC 7950 sec 7.10](https://tools.ietf.org/html/rfc7950#section-7.10)) */
};

typedef union sr_val_data_u sr_val_data_t;

/**
 * @brief Structure that contains value of an data element stored in the sysrepo datastore.
 */
struct sr_val_s {
    /** [XPath](@ref paths) (or rather path) identifier of the data element. */
    char *xpath;

    /** Type of an element. */
    sr_val_type_t type;

    /**
     * Flag for node with default value (applicable only for leaves).
     * It is set to TRUE only if the value was *implicitly* set by the datastore as per
     * module schema. Explicitly set/modified data element (through the sysrepo API) always
     * has this flag unset regardless of the entered value.
     */
    int dflt;

    /** [Origin](@ref oper_ds) of the value. */
    char *origin;

    /** Data of an element (if applicable), properly set according to the type. */
    sr_val_data_t data;
};

typedef struct sr_val_s sr_val_t;

/**
 * @brief Flags used to override default data get behaviour on ::SR_DS_OPERATIONAL by ::sr_get_data call.
 */
typedef enum {
    SR_OPER_DEFAULT = 0,             /**< No special behaviour. */
    SR_OPER_NO_STATE = 1,            /**< Return only configuration data. */
    SR_OPER_NO_CONFIG = 2,           /**< Return only state data. If there are some state subtrees with configuration
                                          parents, these are also returned (with keys if lists). */
    SR_OPER_NO_SUBS = 4,             /**< Return only stored operational data (push), do not call subscriber callbacks (pull). */
    SR_OPER_NO_STORED = 8,           /**< Do not merge with stored operational data (push). */
    SR_OPER_WITH_ORIGIN = 16         /**< Return data with their [origin attributes](@ref datastores). Nodes without
                                          one inherit the origin from parents. */
} sr_get_oper_flag_t;

/**
 * @brief Options overriding default get handling by ::sr_get_data call,
 * it is supposed to be bitwise OR-ed value of any ::sr_get_oper_flag_t flags.
 */
typedef uint32_t sr_get_oper_options_t;

/** @} getdata */

/**
 * @ingroup edit_data_api
 * @{
 */

/**
 * @brief Flags used to override default behavior of data manipulation calls.
 */
typedef enum {
    SR_EDIT_DEFAULT = 0,        /**< Default behavior - non-strict. */
    SR_EDIT_NON_RECURSIVE = 1,  /**< Non-recursive behavior:
                                     by ::sr_set_item, all preceding nodes (parents) of the identified element must exist. */
    SR_EDIT_STRICT = 2,         /**< Strict behavior:
                                     by ::sr_set_item the identified element must not exist (similar to NETCONF create operation),
                                     by ::sr_delete_item the identified element must exist (similar to NETCONF delete operation). */
    SR_EDIT_ISOLATE = 4         /**< Create new operation separately, independent of all the previous operations. Since all the
                                     operations are concatenated into one edit tree, it may happen that 2 incompatible operations
                                     are set and an error is observed. This flag can in those cases be used. Also, if an error
                                     is returned the previous edit is always left untouched. */
} sr_edit_flag_t;

/**
 * @brief Options overriding default behavior of data manipulation calls,
 * it is supposed to be bitwise OR-ed value of any ::sr_edit_flag_t flags.
 */
typedef uint32_t sr_edit_options_t;

/**
 * @brief Options for specifying move direction of ::sr_move_item call.
 */
typedef enum {
    SR_MOVE_BEFORE = 0,    /**< Move the specified item before the selected sibling. */
    SR_MOVE_AFTER = 1,     /**< Move the specified item after the selected. */
    SR_MOVE_FIRST = 2,     /**< Move the specified item to the position of the first child. */
    SR_MOVE_LAST = 3       /**< Move the specified item to the position of the last child. */
} sr_move_position_t;

/** @} editdata */

/**
 * @ingroup subs_api
 * @{
 */

/**
 * @brief Flags used to override default handling of subscriptions.
 */
typedef enum {
    /**
     * @brief Default behavior of the subscription. In case of ::sr_module_change_subscribe call it means that:
     *
     * - for every new subscription structure a thread is created that listens for new events (can be changed
     *   with ::SR_SUBSCR_NO_THREAD flag),
     * - the subscriber is the "owner" of the subscribed data tree and it will appear in the operational
     *   datastore while this subscription is alive (if not already, can be changed using ::SR_SUBSCR_PASSIVE flag),
     * - the callback will be called twice, once with ::SR_EV_CHANGE event and once with ::SR_EV_DONE / ::SR_EV_ABORT
     *   event passed in (can be changed with ::SR_SUBSCR_DONE_ONLY flag).
     */
    SR_SUBSCR_DEFAULT = 0,

    /**
     * @brief There will be no thread created for handling this subscription meaning no event will be processed!
     * Use this flag when the application has its own event loop and it will listen for and process events manually
     * (see ::sr_get_event_pipe and ::sr_subscription_process_events).
     */
    SR_SUBSCR_NO_THREAD = 1,

    /**
     * @brief The subscriber is not the "owner" of the subscribed data tree, just a passive watcher for changes.
     * When this option is passed in to ::sr_module_change_subscribe, the subscription will have no effect on
     * the presence of the subtree in the operational datastore.
     */
    SR_SUBSCR_PASSIVE = 2,

    /**
     * @brief The subscriber does not support verification of the changes and wants to be notified only after
     * the changes has been applied in the datastore, without the possibility to deny them
     * (it will not receive ::SR_EV_CHANGE nor ::SR_EV_ABORT but only ::SR_EV_DONE events).
     */
    SR_SUBSCR_DONE_ONLY = 4,

    /**
     * @brief The subscriber wants to be notified about the current configuration at the moment of subscribing.
     * It will receive ::SR_EV_ENABLED event, whose applying can fail causing the whole subscription to fail.
     * On success this event will be followed by ::SR_EV_DONE. Be careful, ::SR_EV_ENABLED will be triggered
     * even if there are no data so there will not be any changes! Also, this event callback is called as part
     * of the subscribe call (in the same thread) unlike other events.
     */
    SR_SUBSCR_ENABLED = 8,

    /**
     * @brief The subscriber will be called before any other subscribers for the particular module
     * with an additional ::SR_EV_UPDATE event and is then allowed to modify the new module data. It can add new changes
     * by calling standard set functions (such as ::sr_set_item_str) on the implicit callback session and returning.
     * Note that you cannot subscribe more callbacks with this flag on one module with the same priority.
     */
    SR_SUBSCR_UPDATE = 16,

    /**
     * @brief Instead of removing any previous existing matching data before getting them from an operational
     * subscription callback, keep them. Then the returned data are merged into the existing data. Accepted
     * only for operational subscriptions.
     */
    SR_SUBSCR_OPER_MERGE = 32,

    /**
     * @brief Suspend the default handler thread before adding the subscription if it is running. In case of the
     * first subscription, start the handler thread suspended. Meaning any events will not be handled until
     * ::sr_subscription_thread_resume() is called.
     */
    SR_SUBSCR_THREAD_SUSPEND = 64

} sr_subscr_flag_t;

/**
 * @brief Sysrepo subscription context returned from sr_*_subscribe calls,
 * it is supposed to be released by the caller using ::sr_unsubscribe call.
 */
typedef struct sr_subscription_ctx_s sr_subscription_ctx_t;

/**
 * @brief Options overriding default behavior of subscriptions,
 * it is supposed to be a bitwise OR-ed value of any ::sr_subscr_flag_t flags.
 */
typedef uint32_t sr_subscr_options_t;

/** @} subs */

/**
 * @ingroup change_subs_api
 * @{
 */

/**
 * @brief Type of the event that has occurred (passed to application callbacks).
 *
 * @note Each change is normally announced twice: first as ::SR_EV_CHANGE event and then as ::SR_EV_DONE or ::SR_EV_ABORT
 * event. If the subscriber does not support verification, it can subscribe only to ::SR_EV_DONE event by providing
 * ::SR_SUBSCR_DONE_ONLY subscription flag. The general rule is that in case the operation fails, only if the subscriber
 * has __successfully__ processed the first event (::SR_EV_CHANGE/::SR_EV_RPC), it will get the second ::SR_EV_ABORT event.
 */
typedef enum {
    SR_EV_UPDATE,  /**< Occurs before any other events and the subscriber can update the apply-changes diff.
                        It is performed by calling `sr_*_item()` or ::sr_edit_batch() on the callback session
                        __without__ calling ::sr_apply_changes() or any other function. */
    SR_EV_CHANGE,  /**< Occurs just before the changes are committed to the datastore,
                        the subscriber is supposed to verify that the changes are valid and can be applied
                        and prepare all resources required for the changes. The subscriber can still deny the changes
                        in this phase by returning an error from the callback. */
    SR_EV_DONE,    /**< Occurs just after the changes have been successfully committed to the datastore,
                        the subscriber can apply the changes now, but it cannot deny the changes in this
                        phase anymore (any returned errors are just logged and ignored). */
    SR_EV_ABORT,   /**< Occurs in case that the commit transaction has failed because one of the verifiers
                        has denied the change (returned an error). The subscriber is supposed to return the managed
                        application to the state before the commit. Any returned errors are just logged and ignored.
                        This event is also generated for RPC subscriptions when a later callback has failed and
                        this one has already successfully processed ::SR_EV_RPC. The callback that failed will __never__
                        get this event! */
    SR_EV_ENABLED, /**< Occurs for subscriptions with the flag ::SR_SUBSCR_ENABLED and is normally followed by
                        ::SR_EV_DONE. It can fail and will also be triggered even when there is no startup configuration
                        (which is different from the ::SR_EV_CHANGE event). Also note that the callback on this event
                        __cannot__ return ::SR_ERR_CALLBACK_SHELVE. */
    SR_EV_RPC      /**< Occurs for a standard RPC execution. If a later callback fails, ::SR_EV_ABORT is generated. */
} sr_event_t;

/**
 * @brief Type of the operation made on an item, used by changeset retrieval in ::sr_get_change_next.
 */
typedef enum {
    SR_OP_CREATED,   /**< The item has been created by the change. */
    SR_OP_MODIFIED,  /**< The value of the item has been modified by the change. */
    SR_OP_DELETED,   /**< The item has been deleted by the change. */
    SR_OP_MOVED      /**< The item has been moved in the subtree by the change (applicable for leaf-lists and user-ordered lists). */
} sr_change_oper_t;

/**
 * @brief Iterator used for retrieval of a changeset using ::sr_get_changes_iter call.
 */
typedef struct sr_change_iter_s sr_change_iter_t;

/**
 * @brief Callback to be called on the event of changing datastore content of the specified module.
 *
 * @note Callback must not modify the same module and datastore change subscriptions, it would result in a deadlock.
 *
 * @param[in] session Implicit session (do not stop) with information about the changed data (retrieved by
 * ::sr_get_changes_iter) and the event originator session IDs.
 * @param[in] sub_id Subscription ID.
 * @param[in] module_name Name of the module where the change has occurred.
 * @param[in] xpath [XPath](@ref paths) used when subscribing, NULL if the whole module was subscribed to.
 * @param[in] event Type of the callback event that has occurred.
 * @param[in] request_id Request ID unique for the specific @p module_name. Connected events
 * for one request (::SR_EV_CHANGE and ::SR_EV_DONE, for example) have the same request ID.
 * @param[in] private_data Private context opaque to sysrepo, as passed to ::sr_module_change_subscribe call.
 * @return User error code (::SR_ERR_OK on success).
 */
typedef int (*sr_module_change_cb)(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *xpath,
        sr_event_t event, uint32_t request_id, void *private_data);

/** @} datasubs */

/**
 * @ingroup rpc_subs_api
 * @{
 */

/**
 * @brief Callback to be called for the delivery of an RPC/action. Data are represented as ::sr_val_t structures.
 *
 * @note Callback must not modify any RPC/action subscriptions, it would result in a deadlock.
 *
 * @param[in] session Implicit session (do not stop) with information about event originator session IDs.
 * @param[in] sub_id Subscription ID.
 * @param[in] xpath Full operation [xpath](@ref paths) identifying the exact RPC/action executed.
 * @param[in] input Array of input parameters.
 * @param[in] input_cnt Number of input parameters.
 * @param[in] event Type of the callback event that has occurred.
 * @param[in] request_id Request ID unique for the specific @p op_path.
 * @param[out] output Array of output parameters. Should be allocated on heap,
 * will be freed by sysrepo after sending of the RPC response.
 * @param[out] output_cnt Number of output parameters.
 * @param[in] private_data Private context opaque to sysrepo, as passed to ::sr_rpc_subscribe call.
 * @return User error code (::SR_ERR_OK on success).
 */
typedef int (*sr_rpc_cb)(sr_session_ctx_t *session, uint32_t sub_id, const char *xpath, const sr_val_t *input,
        const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt,
        void *private_data);

/**
 * @brief Callback to be called for the delivery of an RPC/action. Data are represented as _libyang_ subtrees.
 *
 * @note Callback must not modify any RPC/action subscriptions, it would result in a deadlock.
 *
 * @param[in] session Implicit session (do not stop) with information about the event originator session IDs.
 * @param[in] sub_id Subscription ID.
 * @param[in] op_path Simple operation [path](@ref paths) identifying the RPC/action.
 * @param[in] input Data tree of input parameters. Always points to the __RPC/action__ itself, even for nested operations.
 * @param[in] event Type of the callback event that has occurred.
 * @param[in] request_id Request ID unique for the specific @p op_path.
 * @param[out] output Data tree for appending any output parameters, the operation root node is provided..
 * @param[in] private_data Private context opaque to sysrepo, as passed to ::sr_rpc_subscribe_tree call.
 * @return User error code (::SR_ERR_OK on success).
 */
typedef int (*sr_rpc_tree_cb)(sr_session_ctx_t *session, uint32_t sub_id, const char *op_path, const struct lyd_node *input,
        sr_event_t event, uint32_t request_id, struct lyd_node *output, void *private_data);

/** @} rpcsubs */

/**
 * @ingroup notif_subs_api
 * @{
 */

/**
 * @brief Type of the notification passed to the ::sr_event_notif_cb and ::sr_event_notif_tree_cb callbacks.
 */
typedef enum {
    SR_EV_NOTIF_REALTIME,         /**< Real-time notification. */
    SR_EV_NOTIF_REPLAY,           /**< Replayed notification. */
    SR_EV_NOTIF_REPLAY_COMPLETE,  /**< Not a real notification, just a signal that the notification replay has completed
                                       (all the stored notifications from the given time interval have been delivered). */
    SR_EV_NOTIF_TERMINATED,       /**< Not a real notification, just a signal that the subscription was terminated,
                                       for whatever reason, it is always the last notification all subscriptions receive. */
    SR_EV_NOTIF_MODIFIED,         /**< Not a real notification, just a signal that the subscription parameters were modified. */
    SR_EV_NOTIF_SUSPENDED,        /**< Not a real notification, just a signal that the subscription was suspended. */
    SR_EV_NOTIF_RESUMED           /**< Not a real notification, just a signal that the subscription was resumed after
                                       previously suspended. */
} sr_ev_notif_type_t;

/**
 * @brief Callback to be called for the delivery of a notification. Data are represented as ::sr_val_t structures.
 *
 * @note Callback must not modify the same module notification subscriptions, it would result in a deadlock.
 *
 * @param[in] session Implicit session (do not stop) with information about the event originator session IDs.
 * @param[in] sub_id Subscription ID.
 * @param[in] notif_type Type of the notification.
 * @param[in] xpath Full operation [xpath](@ref paths) identifying the exact notification executed.
 * @param[in] values Array of all nodes that hold some data in event notification subtree.
 * @param[in] values_cnt Number of items inside the values array.
 * @param[in] timestamp Time when the notification was generated
 * @param[in] private_data Private context opaque to sysrepo, as passed to ::sr_notif_subscribe call.
 */
typedef void (*sr_event_notif_cb)(sr_session_ctx_t *session, uint32_t sub_id, const sr_ev_notif_type_t notif_type,
        const char *xpath, const sr_val_t *values, const size_t values_cnt, struct timespec *timestamp, void *private_data);

/**
 * @brief Callback to be called for the delivery of a notification. Data are represented as _libyang_ subtrees.
 *
 * @note Callback must not modify the same module notification subscriptions, it would result in a deadlock.
 *
 * @param[in] session Implicit session (do not stop) with information about the event originator session IDs.
 * @param[in] sub_id Subscription ID.
 * @param[in] notif_type Type of the notification.
 * @param[in] notif Notification data tree. Always points to the __notification__ itself, even for nested ones.
 * @param[in] timestamp Time when the notification was generated
 * @param[in] private_data Private context opaque to sysrepo, as passed to ::sr_notif_subscribe_tree call.
 */
typedef void (*sr_event_notif_tree_cb)(sr_session_ctx_t *session, uint32_t sub_id, const sr_ev_notif_type_t notif_type,
        const struct lyd_node *notif, struct timespec *timestamp, void *private_data);

/** @} notifsubs */

/**
 * @ingroup oper_subs_api
 * @{
 */

/**
 * @brief Callback to be called when operational data at the selected xpath are requested.
 * Data are represented as _libyang_ subtrees.
 *
 * When the callback is called, the data parent is provided. Any parent children (selected by @p path)
 * are removed and should be provided by the callback instead, if they exist. Callback handler can
 * provide any data matching the @p path but in case there are other nested subscriptions,
 * they will be called after this one (and when they are called, their parent children will again be removed
 * which can result in nodes provided by the original callback being lost).
 *
 * @note Callback must not modify the same module operational subscriptions, it would result in a deadlock.
 *
 * @param[in] session Implicit session (do not stop) with information about the event originator session IDs.
 * @param[in] sub_id Subscription ID.
 * @param[in] module_name Name of the affected module.
 * @param[in] path [Path](@ref paths) identifying the subtree that is supposed to be provided, same as the one used
 * for the subscription.
 * @param[in] request_xpath [XPath](@ref paths) as requested by a client. Can be NULL.
 * @param[in] request_id Request ID unique for the specific @p module_name.
 * @param[in,out] parent Pointer to an existing parent of the requested nodes. Is NULL for top-level nodes.
 * Caller is supposed to append the requested nodes to this data subtree and return either the original parent
 * or a top-level node.
 * @param[in] private_data Private context opaque to sysrepo, as passed to ::sr_oper_get_subscribe call.
 * @return User error code (::SR_ERR_OK on success).
 */
typedef int (*sr_oper_get_items_cb)(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path,
        const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

/** @} oper_subs */

/**
 * @ingroup plugin_api
 * @{
 */

/**
 * @brief Sysrepo plugin initialization callback.
 *
 * @param[in] session Sysrepo session that can be used for any API calls needed
 * for plugin initialization (mainly for reading of startup configuration
 * and subscribing for notifications).
 * @param[out] private_data Private context (opaque to sysrepo) that will be
 * passed to ::srp_cleanup_cb_t when plugin cleanup is requested.
 *
 * @return Error code (::SR_ERR_OK on success).
 */
typedef int (*srp_init_cb_t)(sr_session_ctx_t *session, void **private_data);

/**
 * @brief Sysrepo plugin cleanup callback.
 *
 * @param[in] session Sysrepo session that can be used for any API calls
 * needed for plugin cleanup (mainly for unsubscribing of subscriptions
 * initialized in ::srp_init_cb_t).
 * @param[in] private_data Private context as passed in ::srp_init_cb_t.
 */
typedef void (*srp_cleanup_cb_t)(sr_session_ctx_t *session, void *private_data);

/** @} plugin */

#ifdef __cplusplus
}
#endif

#endif /* _SYSREPO_TYPES_H */
