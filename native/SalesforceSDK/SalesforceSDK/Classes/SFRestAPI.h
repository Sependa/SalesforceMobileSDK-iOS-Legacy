/*
 Copyright (c) 2011, salesforce.com, inc. All rights reserved.
 
 Redistribution and use of this software in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this list of conditions
    and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of
    conditions and the following disclaimer in the documentation and/or other materials provided
    with the distribution.
  * Neither the name of salesforce.com, inc. nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific prior written
    permission of salesforce.com, inc.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <Foundation/Foundation.h>
#import "SFRestRequest.h"

/*
 * Domain used for errors reported by the rest API (non HTTP errors)
 * (for example, passing an invalid SOQL string when doing a query)
 */
extern NSString* const kSFRestErrorDomain;
/*
 * Error code used for all rest API errors (non HTTP errors)
 * (for example, passing an invalid SOQL string when doing a query)
 */
extern NSInteger const kSFRestErrorCode;


/*
 * Default API version (currently "v23.0")
 * You can override this by using setApiVersion:
 */
extern NSString* const kSFRestDefaultAPIVersion;

/*
 * The designator used to signify that the app is a native app.
 */
extern NSString * const kSFMobileSDKNativeDesignator;

/*
 * The current version of the native Mobile SDK.
 */
extern NSString * const kSFMobileSDKVersion;

@class SFOAuthCoordinator;
@class RKClient;

/**
 Main class used to issue REST requests to the standard Force.com REST API.
 
 See the [Force.com REST API Developer's Guide](http://www.salesforce.com/us/developer/docs/api_rest/index.htm)
 for more information regarding the Force.com REST API.

 ## Initialization
 
 This class is a singleton, and can be accessed by referencing [SFRestAPI sharedInstance].  It relies
 upon the shared credentials managed by SFAccountManager, for forming up and sending authenticated
 REST requests.
 
 ## Sending requests

 Sending a request is done using `send:delegate:`.
 The class sending the request has to conform to the protocol `SFRestDelegate`.
 
 A request can be obtained in two different ways:

 - by calling the appropriate `requestFor[...]` method

 - by building the `SFRestRequest` manually
 
 Note: If you opt to build an SFRestRequest manually, you should be aware that
 send:delegate: expects that if the request.path does not begin with the
 request.endpoint prefix, it will add the request.endpoint prefix 
 (kSFDefaultRestEndpoint by default) to the request path.
 
 
 For example, this sample code calls the `requestForDescribeWithObjectType:` method to return
 information about the Account object.

    - (void)describeAccount {
        SFRestRequest *request = [[SFRestAPI sharedInstance]
                                  requestForDescribeWithObjectType:@"Account"];
        [[SFRestAPI sharedInstance] send:request delegate:self];
    }
 
    #pragma mark - SFRestDelegate
 
    - (void)request:(SFRestRequest *)request didLoadResponse:(id)jsonResponse {
        NSDictionary *dict = (NSDictionary *)jsonResponse;
        NSArray *fields = (NSArray *)[dict objectForKey:@"fields"];
        // ...
    }
 
    - (void)request:(SFRestRequest*)request didFailLoadWithError:(NSError*)error {
        // handle error
    }
 
    - (void)requestDidCancelLoad:(SFRestRequest *)request {
        // handle error
    }
 
    - (void)requestDidTimeout:(SFRestRequest *)request {
        // handle error
    }
 
 ## Error handling
 
 When sending a `SFRestRequest`, you may encounter one of these errors:

 - The request parameters could be invalid (for instance, passing `nil` to the `requestForQuery:`,
 or trying to update a non-existent object).
 In this case, `request:didFailLoadWithError:` is called on the `SFRestDelegate`.
 The error passed will have an error domain of `kSFRestErrorDomain`
 
 - The oauth access token (session ID) managed by SFAccountManager could have expired.
 In this case, the framework tries to acquire another access token and re-issue
 the `SFRestRequest`. This is all done transparently and the appropriate delegate method
 is called once the second `SFRestRequest` returns. 

 - Requesting a new access token (session ID) could fail (if the access token has expired
 and the OAuth refresh token is invalid).
 In this case, `request:didFailLoadWithError:` will be called on the `SFRestDelegate`.
 The error passed will have an error domain of `kSFOAuthErrorDomain`.
 Note that this is a very rare case.

 - The underlying HTTP request could fail (Salesforce server is innaccessible...)
 In this case, `request:didFailLoadWithError:` is called on the `SFRestDelegate`.
 The error passed will be a standard `RestKit` error with an error domain of `RKRestKitErrorDomain`. 

 */
@interface SFRestAPI : NSObject {
    RKClient *_client;
    NSString *_apiVersion;
}

@property(nonatomic) BOOL verboseLogging;

/**
 * Gets or sets the value of the `SFOAuthCoordinator` instance associated with SFRestAPI requests.
 * Note: If your app inherits from `SFNativeRestAppDelegate`, or you otherwise manage the account
 * configuration of your app through `SFAccountManager`, you should not need to set/configure this
 * property, as it's closely aligned with SFAccountManager already.
 */
@property (nonatomic, strong) SFOAuthCoordinator *coordinator;

/**
 * Property exposing the RestKit `RKClient` instance associated with this object.
 */
@property (nonatomic, readonly) RKClient *rkClient;

/**
 * The REST API version used for all the calls. This could be "v21.0", "v22.0"...
 * The default value is `kSFRestDefaultAPIVersion` (currently "v23.0")
 */
@property (nonatomic, strong) NSString *apiVersion;

/**
 * Returns the singleton instance of `SFRestAPI`
 * Dependent on authenticated credentials in SFAccountManager, to properly form up
 * authenticated requests.
 */
+ (SFRestAPI *)sharedInstance;

/**
* Will cancel given request.
*/
- (void)cancelRequest:(SFRestRequest *)request;

/**
 * Sends a REST request to the Salesforce server and invokes the appropriate delegate method.
 * @param request the SFRestRequest to be sent
 * @param delegate the delegate object used when the response from the server is returned. 
 * This overwrites the delegate property of the request.
 */
- (void)send:(SFRestRequest *)request delegate:(id<SFRestDelegate>)delegate;

///---------------------------------------------------------------------------------------
/// @name SFRestRequest factory methods
///---------------------------------------------------------------------------------------

/**
 * Returns an `SFRestRequest` which lists summary information about each
 * Salesforce.com version currently available, including the version, 
 * label, and a link to each version's root.
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_versions.htm
 */
- (SFRestRequest *)requestForVersions;

/**
 * Returns an `SFRestRequest` which lists available resources for the
 * client's API version, including resource name and URI.
 * @see Rest API link: http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_discoveryresource.htm
 */
- (SFRestRequest *)requestForResources;

/**
 * Returns an `SFRestRequest` which lists the available objects and their
 * metadata for your organization's data.
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_describeGlobal.htm
 */
- (SFRestRequest *)requestForDescribeGlobal;

/**
 * Returns an `SFRestRequest` which Describes the individual metadata for the
 * specified object.
 * @param objectType object type; for example, "Account"
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_basic_info.htm
 */
- (SFRestRequest *)requestForMetadataWithObjectType:(NSString *)objectType;

/**
 * Returns an `SFRestRequest` which completely describes the individual metadata
 * at all levels for the 
 * specified object.
 * @param objectType object type; for example, "Account"
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_describe.htm
 */
- (SFRestRequest *)requestForDescribeWithObjectType:(NSString *)objectType;

/**
 * Returns an `SFRestRequest` which retrieves field values for a record of the given type.
 * @param objectType object type; for example, "Account"
 * @param objectId the record's object ID
 * @param fieldList comma-separated list of fields for which 
 *               to return values; for example, "Name,Industry,TickerSymbol".
 *               Pass nil to retrieve all the fields.
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_retrieve.htm
 */
- (SFRestRequest *)requestForRetrieveWithObjectType:(NSString *)objectType
                                           objectId:(NSString *)objectId 
                                          fieldList:(NSString *)fieldList;

/**
 * Returns an `SFRestRequest` which creates a new record of the given type.
 * @param objectType object type; for example, "Account"
 * @param fields an NSDictionary containing initial field names and values for 
 *               the record, for example, {Name: "salesforce.com", TickerSymbol: 
 *               "CRM"}
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_retrieve.htm
 */
- (SFRestRequest *)requestForCreateWithObjectType:(NSString *)objectType 
                                           fields:(NSDictionary *)fields;

/**
 * Returns an `SFRestRequest` which creates or updates record of the given type, based on the 
 * given external ID.
 * @param objectType object type; for example, "Account"
 * @param externalIdField external ID field name; for example, "accountMaster__c"
 * @param externalId the record's external ID value
 * @param fields an NSDictionary containing field names and values for 
 *               the record, for example, {Name: "salesforce.com", TickerSymbol 
 *               "CRM"}
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_upsert.htm
 */
- (SFRestRequest *)requestForUpsertWithObjectType:(NSString *)objectType
                                  externalIdField:(NSString *)externalIdField
                                       externalId:(NSString *)externalId
                                           fields:(NSDictionary *)fields;

/**
 * Returns an `SFRestRequest` which updates field values on a record of the given type.
 * @param objectType object type; for example, "Account"
 * @param objectId the record's object ID
 * @param fields an object containing initial field names and values for 
 *               the record, for example, {Name: "salesforce.com", TickerSymbol 
 *               "CRM"}
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_retrieve.htm
 */
- (SFRestRequest *)requestForUpdateWithObjectType:(NSString *)objectType 
                                         objectId:(NSString *)objectId
                                           fields:(NSDictionary *)fields;

/**
 * Returns an `SFRestRequest` which deletes a record of the given type.
 * @param objectType object type; for example, "Account"
 * @param objectId the record's object ID
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_sobject_retrieve.htm
 */
- (SFRestRequest *)requestForDeleteWithObjectType:(NSString *)objectType 
                                         objectId:(NSString *)objectId;

/**
 * Returns an `SFRestRequest` which executes the specified SOQL query.
 * @param soql a string containing the query to execute - for example, "SELECT Id, 
 *             Name from Account ORDER BY Name LIMIT 20"
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_query.htm
 */
- (SFRestRequest *)requestForQuery:(NSString *)soql;

/**
 * Returns an `SFRestRequest` which executes the specified SOSL search.
 * @param sosl a string containing the search to execute - for example, "FIND {needle}"
 * @see http://www.salesforce.com/us/developer/docs/api_rest/Content/resources_search.htm
 */
- (SFRestRequest *)requestForSearch:(NSString *)sosl;


///---------------------------------------------------------------------------------------
/// @name Other utility methods
///---------------------------------------------------------------------------------------

/**
 * Provides the User-Agent string used by the SDK
 */
+ (NSString *)userAgentString;


@end
