Pod::Spec.new do |s|
  s.name         = "SalesforceMobileSDK-iOS-Taptera"
  s.version      = "1.0.0"
  s.summary      = "Taptera version of Salesforce mobile SDK for iOS."
  s.homepage     = "https://github.com/Taptera/SalesforceMobileSDK-iOS.git"
  s.license      = 'Taptera'
  s.author       = { "Taptera" => "ios-devs@taptera.com" }
  s.source       = { :git => "git@github.com:Taptera/SalesforceMobileSDK-iOS.git", :tag => "cocoapods_ver#{s.version}" }
  s.platform     = :ios, '4.3'

  s.source_files = 'dist/SalesforceSDK/SalesforceSDK/include/SalesforceSDK/**/*.{h}'
  s.library = 'SalesforceSDK'
  s.preserve_paths = 'dist/SalesforceSDK/SalesforceSDK/libSalesforceSDK.a'
  s.xcconfig  =  { 'LIBRARY_SEARCH_PATHS' => '"$(PODS_ROOT)/SalesforceMobileSDK-iOS-Taptera/dist/SalesforceSDK/SalesforceSDK"' }
  s.dependency 'RestKit', '~> 0.10.2'

  s.subspec 'oAuth' do |ss|
    ss.source_files = 'dist/SalesforceOAuth/SalesforceOAuth/Headers/SalesforceOAuth/**/*.{h}'
    ss.library = 'SalesforceOAuth'
    ss.preserve_paths = 'dist/SalesforceOAuth/SalesforceOAuth/Libraries/libSalesforceOAuth.a'
    ss.xcconfig  =  { 'LIBRARY_SEARCH_PATHS' => '"$(PODS_ROOT)/SalesforceMobileSDK-iOS-Taptera/dist/SalesforceOAuth/SalesforceOAuth/Libraries"' }
  end

  s.subspec 'CommonUtils' do |ss|
    ss.source_files = 'dist/SalesforceCommonUtils/SalesforceCommonUtils/Headers/*.{h}'
    ss.library = 'SalesforceCommonUtils'
    ss.preserve_paths = 'dist/SalesforceCommonUtils/SalesforceCommonUtils/Libraries/libSalesforceCommonUtils.a'
    ss.xcconfig  =  { 'LIBRARY_SEARCH_PATHS' => '"$(PODS_ROOT)/SalesforceMobileSDK-iOS-Taptera/dist/SalesforceCommonUtils/SalesforceCommonUtils/Libraries"' }
  end

  s.subspec 'PrivateHeaders' do |ss|
    ss.source_files = 'shared/Classes/**/*.{h}'
  end

end
