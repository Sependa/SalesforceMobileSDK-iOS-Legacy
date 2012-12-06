Pod::Spec.new do |s|
  s.name         = "SalesforceMobileSDK-iOS-Taptera"
  s.version      = "1.0.0"
  s.summary      = "Taptera version of Salesforce mobile SDK for iOS."
  s.homepage     = "https://github.com/Taptera/SalesforceMobileSDK-iOS.git"
  s.license      = 'Taptera'
  s.author       = { "Taptera" => "ios-devs@taptera.com" }
  s.source       = { :git => "git@github.com:Taptera/SalesforceMobileSDK-iOS.git", :tag => "v#{s.version}" }
  s.platform     = :ios, '4.3'

  s.source_files = 'dist/SalesforceSDK/SalesforceSDK/include/SalesforceSDK/**/*.{h}'
  s.resources = 'dist/SalesforceSDK/SalesforceSDK/libSalesforceSDK.a'

  s.subspec 'oAuth' do |ss|
    ss.source_files = 'dist/SalesforceOAuth/SalesforceOAuth/Headers/SalesforceOAuth/**/*.{h}'
    ss.resources = 'dist/SalesforceOAuth/SalesforceOAuth/Libraries/libSalesforceOAuth.a'
  end

  s.subspec 'PrivateHeaders' do |ss|
    ss.source_files = 'shared/Classes/**/*.{h}'
  end

end
