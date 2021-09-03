//
//  AMSocketManager.m
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

//#pragma once
#import "AMSocketManager.h"
#import "AMSocketHeader.h"
#import <GCDAsyncSocket.h>
#import "EccCode.h"

@interface AMSocketManager () <GCDAsyncSocketDelegate>

@property (nonatomic, strong) GCDAsyncSocket *socket;

@end

@implementation AMSocketManager

static AMSocketManager *_manager = nil;

+ (AMSocketManager *)shared {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        if (!_manager) {
            _manager = [[self alloc] init];
        }
    });
    return _manager;
}

- (BOOL)connectToRemoteServer {
    if (!_socket) {
        _socket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:dispatch_get_global_queue(0, 0)];
    }
    
    if (!_socket.isConnected) {
        NSError *error = nil;
        BOOL result = [_socket connectToHost:kSocketIP onPort:kSocketPort withTimeout:5 error:&error];
        if (result) {
            NSLog(@"连接控制服务器成功~");
        }else {
            NSLog(@"连接控制服务器失败：%@", error.localizedDescription);
        }
        return result;
    }
    return NO;
}



//MARK: GCGAsyncSocketDelegate


/**
 * Called when a socket accepts a connection.
 * Another socket is automatically spawned to handle it.
 *
 * You must retain the newSocket if you wish to handle the connection.
 * Otherwise the newSocket instance will be released and the spawned connection will be closed.
 *
 * By default the new socket will have the same delegate and delegateQueue.
 * You may, of course, change this at any time.
**/
- (void)socket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket {
    NSLog(@"-- %s --", __func__);
}

/**
 * Called when a socket connects and is ready for reading and writing.
 * The host parameter will be an IP address, not a DNS name.
**/
- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(uint16_t)port {
    NSLog(@"-- %s --, %@:%hu", __func__, host, port);
    
    [self.socket readDataWithTimeout:-1 tag:kSocketTag];

}






//void InitECCKey()
//{
//
//    char                        m_szPublicKeyX[ECC_KEY_LEN];
//    char                        m_szPublicKeyY[ECC_KEY_LEN];
//    char                        m_szLittleKey[ECC_KEY_LEN];
//
//    char                        m_szRKeyX[ECC_KEY_LEN];
//    char                        m_szRKeyY[ECC_KEY_LEN];
//    char                        m_szPrivateKey[ECC_KEY_LEN];
//
//    std::string strKey = ("FDEAB7BEB6119B28AF2C8069690DFD9E31826941CE38AB49");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPublicKeyX, ECC_KEY_LEN);
//
//    strKey = ("310B70A07716300EFC2D6761B3975E2AEC4E4C6097A7D12C");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPublicKeyY, ECC_KEY_LEN);
//
//    strKey = ("8E0A669C5A185F21119BC3D5CFEB11A293EAB39325AC9D48");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szLittleKey, ECC_KEY_LEN);
//
//    strKey = ("EBDCA421B0A437B7A059D3EFC94B353B771F84DC09B6FFD6");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szRKeyX, ECC_KEY_LEN);
//
//    strKey = ("A2564A78C9C6E3CF7983FDF89F0F66BC9927B2CAC98973E4");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szRKeyY, ECC_KEY_LEN);
//
//    strKey = ("D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF88920");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPrivateKey, ECC_KEY_LEN);
//
//#ifdef _DEBUG
//    UCHAR szInputPlaintext[47] = "D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF889";
//    UCHAR szOutputCiphertext[49] = { 0 };
//    UCHAR szOutputPlaintext[49] = { 0 };
//    ecc_enc((unsigned char *)m_szPublicKeyX, (unsigned char *)m_szPublicKeyY, (unsigned char *)m_szLittleKey,
//        szInputPlaintext, 46, szOutputCiphertext, 49);
//
//    ecc_dec((unsigned char *)m_szRKeyX, (unsigned char *)m_szRKeyY, (unsigned char *)m_szPrivateKey,
//        szOutputCiphertext, 48, szOutputPlaintext, 49);
//
//#endif
//}

- (NSData *)encrypt:(NSString *)content {
    NSData *dataToSign = [content dataUsingEncoding:NSUTF8StringEncoding];
    
    //设置属性
    NSDictionary *options = @{(id)kSecClass : (id)kSecClassKey,
                             (id)kSecAttrKeyType : (id)kSecAttrKeyTypeEC,
                             (id)kSecAttrKeySizeInBits : @24,
                             (id)kSecAttrLabel : @"MGgCAQEEHHIh0vvsympSecVtSk6w3BueBV8kEOEfKk6sfxKgBwYFK4EEACGhPAM6AARLW5dthIQc0D1W56HcEfnpenelFNVSd9ymXnzz+/ZqFOtgFGWKMytu0iPaai7BjmAArVw2jSHP1g==",
                             (id)kSecReturnRef : @YES};
    //错误保存
    CFErrorRef error = NULL;
//    SecKeyRef privateKey = SecKeyCreateRandomKey((__bridge CFDictionaryRef)options, &error);
    
    SecKeyRef privateKey = (__bridge SecKeyRef)(CFBridgingRelease(SecKeyCreateRandomKey((__bridge CFDictionaryRef)options, &error)));
    
    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)options, (CFTypeRef *)&privateKey);
    
    NSData *signature = CFBridgingRelease(SecKeyCreateSignature(privateKey, kSecKeyAlgorithmECDSASignatureMessageX962SHA224, (CFDataRef)dataToSign, &error));
    return signature;
}

- (NSString *)decode:(NSData *)data {
    //设置属性
    NSDictionary *options = @{(id)kSecClass : (id)kSecClassKey,
                             (id)kSecAttrKeyType : (id)kSecAttrKeyTypeECSECPrimeRandom,
                             (id)kSecAttrKeySizeInBits : @24,
                             (id)kSecAttrLabel : @"ME4wEAYHKoZIzj0CAQYFK4EEACEDOgAES1uXbYSEHNA9Vueh3BH56Xp3pRTVUnfcpl588/v2ahTrYBRlijMrbtIj2mouwY5gAK1cNo0hz9Y=",
                             (id)kSecReturnRef : @YES};
    //错误保存
    CFErrorRef error = NULL;
    
    //根据属性生成key
    SecKeyRef publicKey = SecKeyCreateWithData((__bridge CFDataRef)data, (__bridge CFDictionaryRef)options, &error);
    
    //参数选择
    SecKeyAlgorithm algorithm = kSecKeyAlgorithmECIESEncryptionCofactorX963SHA224AESGCM;
    //错误保存
    error = NULL;
    //数据解密
    NSData *dt = (__bridge NSData *)CFBridgingRetain(CFBridgingRelease(SecKeyCreateDecryptedData(publicKey, algorithm, (__bridge CFDataRef)data, &error)));
    
    NSString *str = [[NSString alloc] initWithData:dt encoding:NSUTF8StringEncoding];
    
    return str;
}

/**
 * Called when a socket has completed reading the requested data into memory.
 * Not called if there is an error.
**/
- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag {
    NSLog(@"-- %s --", __func__);
    
    

    
    
    
    
    return;
    NSDictionary *parameters = @{
                                 (__bridge id)kSecAttrKeyType: (__bridge id)kSecAttrKeyTypeEC,
                                 (__bridge id)kSecAttrKeySizeInBits: @256,
                                 (__bridge id)kSecPrivateKeyAttrs: @{
                                         (__bridge id)kSecAttrIsPermanent: @YES,
                                         (__bridge id)kSecAttrApplicationTag: [@"MGgCAQEEHHIh0vvsympSecVtSk6w3BueBV8kEOEfKk6sfxKgBwYFK4EEACGhPAM6AARLW5dthIQc0D1W56HcEfnpenelFNVSd9ymXnzz+/ZqFOtgFGWKMytu0iPaai7BjmAArVw2jSHP1g==" dataUsingEncoding:NSUTF8StringEncoding],
                                         },
                                 (__bridge id)kSecPublicKeyAttrs: @{
                                         (__bridge id)kSecAttrIsPermanent: @YES,
                                         (__bridge id)kSecAttrApplicationTag: [@"ME4wEAYHKoZIzj0CAQYFK4EEACEDOgAES1uXbYSEHNA9Vueh3BH56Xp3pRTVUnfcpl588/v2ahTrYBRlijMrbtIj2mouwY5gAK1cNo0hz9Y=" dataUsingEncoding:NSUTF8StringEncoding],
                                         }
                                 };

    SecKeyRef publicKey, privateKey;
    OSStatus status = SecKeyGeneratePair((__bridge CFDictionaryRef)parameters, &publicKey, &privateKey);
    
    NSData *dt = [@"时代峰峻阿里" dataUsingEncoding:NSUTF8StringEncoding];
    
    CFErrorRef error;
    NSData *signature = CFBridgingRelease(SecKeyCreateSignature(privateKey, kSecKeyAlgorithmECDSASignatureMessageX962SHA224, (CFDataRef)dt, &error));
    NSLog(@"ddd");
    
    SecItemCopyMatching((__bridge CFDictionaryRef)parameters, (CFTypeRef *)&privateKey);
    BOOL canEncrypt = SecKeyIsAlgorithmSupported((SecKeyRef)privateKey, kSecKeyOperationTypeEncrypt, kSecKeyAlgorithmECDSASignatureMessageX962SHA224);
    //数据解密
    NSData *dt2 = (__bridge NSData *)CFBridgingRetain(CFBridgingRelease(SecKeyCreateDecryptedData(publicKey, kSecKeyAlgorithmECDSASignatureMessageX962SHA224, (__bridge CFDataRef)data, &error)));
    
    NSString *str = [[NSString alloc] initWithData:dt2 encoding:NSUTF8StringEncoding];
    NSLog(@"%@", str);
    return;
    
    
//    NSData *d = [self encrypt:@"啦啦啦啦啦哈哈哈哈"];
    
//    NSString *str = [self decode:d];

//    NSLog(@"%@", str);
    
//    NSError *error;
//    SecAccessControlRef access = SecAccessControlCreateWithFlags(kCFAllocatorDefault, kSecAttrAccessibleWhenPasscodeSetThisDeviceOnly, kSecAccessControlPrivateKeyUsage, (void *)&error);
    
    
    
    
    
    
    
    
    
//    SecKeyRef privateKey = SecKeyCreateRandomKey((__bridge CFDictionaryRef)options, (void*)&error);
    
//    SecKeyRef privateKey = CFBridgingRetain(CFBridgingRelease(SecKeyCreateRandomKey((__bridge CFDictionaryRef)params, (void *)&error)));
    
//    SecKeyRef privateKey;
//    OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)params, (CFTypeRef *)&privateKey);
//
//    error = nil;
//    NSData *dataToSign = [@"我是签名内容" dataUsingEncoding:NSUTF8StringEncoding];
//
//    NSData *signature = CFBridgingRelease(SecKeyCreateSignature(privateKey, kSecKeyAlgorithmECDSASignatureMessageX962SHA256, (CFDataRef)dataToSign, (void *)&error));
    


    
//    SecKeyRef publicKey = SecKeyCopyPublicKey(privateKey);

//    Boolean verified = SecKeyVerifySignature((SecKeyRef)publicKey, kSecKeyAlgorithmECDSASignatureMessageX962SHA256, (CFDataRef)dataToSign, (CFDataRef)signature, (void *)&error);
//
//
//    NSString *message;
//    if (verified == 1) {
//        message = [NSString stringWithFormat:@"signature:%@ verified:%@ error:%@", signature, @"验签成功", error];
//    }else {
//        message = [NSString stringWithFormat:@"signature:%@ verified:%@ error:%@", signature, @"验签失败", error];
//    }
    
    
    
//    BOOL canEncrypt = SecKeyIsAlgorithmSupported((SecKeyRef)publicKey, kSecKeyOperationTypeEncrypt, algorithm); //加密
    
    
    
    
    
}

//void CRCSvrProxy::OnAESEncipherKey(CDataPacket* pDataPacket)
//{
//    assert(nullptr != pDataPacket);
//    if (nullptr != pDataPacket)
//    {
//        RCP::AES_ENCIPHER_KEY_REQUEST* pRequest = (RCP::AES_ENCIPHER_KEY_REQUEST*)pDataPacket->GetHeaderPtr();
//        ecc_dec((unsigned char *)m_szRKeyX
//            , (unsigned char *)m_szRKeyY
//            , (unsigned char *)m_szPrivateKey
//            , (unsigned char *)pRequest->szKeyData
//            , 48
//            , (unsigned char *)pRequest->szKeyData
//            , 48);
//
//        SetAESEncipherKey((unsigned char *)pRequest->szKeyData);
//        SendCommonResponse(pDataPacket, true);
//        SetConversationStatus(CConsoleProxy::RPS_ESTABLISHMENT);
//    }
//}


//void CRCSvrProxy::InitECCKey()
//{
//    std::string strKey = ("FDEAB7BEB6119B28AF2C8069690DFD9E31826941CE38AB49");
//    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPublicKeyX, ECC_KEY_LEN);
//
//    strKey = ("310B70A07716300EFC2D6761B3975E2AEC4E4C6097A7D12C");
//    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPublicKeyY, ECC_KEY_LEN);
//
//    strKey = ("8E0A669C5A185F21119BC3D5CFEB11A293EAB39325AC9D48");
//    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szLittleKey, ECC_KEY_LEN);
//
//    strKey = ("EBDCA421B0A437B7A059D3EFC94B353B771F84DC09B6FFD6");
//    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szRKeyX, ECC_KEY_LEN);
//
//    strKey = ("A2564A78C9C6E3CF7983FDF89F0F66BC9927B2CAC98973E4");
//    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szRKeyY, ECC_KEY_LEN);
//
//    strKey = ("D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF88920");
//    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPrivateKey, ECC_KEY_LEN);
//
//#ifdef _DEBUG
//    UCHAR szInputPlaintext[47] = "D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF889";
//    UCHAR szOutputCiphertext[49] = { 0 };
//    UCHAR szOutputPlaintext[49] = { 0 };
//    ecc_enc((unsigned char *)m_szPublicKeyX, (unsigned char *)m_szPublicKeyY, (unsigned char *)m_szLittleKey,
//        szInputPlaintext, 46, szOutputCiphertext, 49);
//
//    ecc_dec((unsigned char *)m_szRKeyX, (unsigned char *)m_szRKeyY, (unsigned char *)m_szPrivateKey,
//        szOutputCiphertext, 48, szOutputPlaintext, 49);
//
//#endif
//}

/**
 * Called when a socket has read in data, but has not yet completed the read.
 * This would occur if using readToData: or readToLength: methods.
 * It may be used for things such as updating progress bars.
**/
- (void)socket:(GCDAsyncSocket *)sock didReadPartialDataOfLength:(NSUInteger)partialLength tag:(long)tag {
    NSLog(@"-- %s --", __func__);
}

/**
 * Called when a socket has completed writing the requested data. Not called if there is an error.
**/
- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag {
    NSLog(@"-- %s --", __func__);
}

/**
 * Called when a socket has written some data, but has not yet completed the entire write.
 * It may be used for things such as updating progress bars.
**/
- (void)socket:(GCDAsyncSocket *)sock didWritePartialDataOfLength:(NSUInteger)partialLength tag:(long)tag {
    NSLog(@"-- %s --", __func__);
}


/**
 * Conditionally called if the read stream closes, but the write stream may still be writeable.
 *
 * This delegate method is only called if autoDisconnectOnClosedReadStream has been set to NO.
 * See the discussion on the autoDisconnectOnClosedReadStream method for more information.
**/
- (void)socketDidCloseReadStream:(GCDAsyncSocket *)sock {
    NSLog(@"-- %s --", __func__);
}

/**
 * Called when a socket disconnects with or without error.
 *
 * If you call the disconnect method, and the socket wasn't already disconnected,
 * then an invocation of this delegate method will be enqueued on the delegateQueue
 * before the disconnect method returns.
 *
 * Note: If the GCDAsyncSocket instance is deallocated while it is still connected,
 * and the delegate is not also deallocated, then this method will be invoked,
 * but the sock parameter will be nil. (It must necessarily be nil since it is no longer available.)
 * This is a generally rare, but is possible if one writes code like this:
 *
 * asyncSocket = nil; // I'm implicitly disconnecting the socket
 *
 * In this case it may preferrable to nil the delegate beforehand, like this:
 *
 * asyncSocket.delegate = nil; // Don't invoke my delegate method
 * asyncSocket = nil; // I'm implicitly disconnecting the socket
 *
 * Of course, this depends on how your state machine is configured.
**/
- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(nullable NSError *)err {
    NSLog(@"-- %s --", __func__);
}

/**
 * Called after the socket has successfully completed SSL/TLS negotiation.
 * This method is not called unless you use the provided startTLS method.
 *
 * If a SSL/TLS negotiation fails (invalid certificate, etc) then the socket will immediately close,
 * and the socketDidDisconnect:withError: delegate method will be called with the specific SSL error code.
**/
- (void)socketDidSecure:(GCDAsyncSocket *)sock {
    NSLog(@"-- %s --", __func__);
}

/**
 * Allows a socket delegate to hook into the TLS handshake and manually validate the peer it's connecting to.
 *
 * This is only called if startTLS is invoked with options that include:
 * - GCDAsyncSocketManuallyEvaluateTrust == YES
 *
 * Typically the delegate will use SecTrustEvaluate (and related functions) to properly validate the peer.
 *
 * Note from Apple's documentation:
 *   Because [SecTrustEvaluate] might look on the network for certificates in the certificate chain,
 *   [it] might block while attempting network access. You should never call it from your main thread;
 *   call it only from within a function running on a dispatch queue or on a separate thread.
 *
 * Thus this method uses a completionHandler block rather than a normal return value.
 * The completionHandler block is thread-safe, and may be invoked from a background queue/thread.
 * It is safe to invoke the completionHandler block even if the socket has been closed.
**/
- (void)socket:(GCDAsyncSocket *)sock didReceiveTrust:(SecTrustRef)trust
completionHandler:(void (^)(BOOL shouldTrustPeer))completionHandler {
    NSLog(@"-- %s --", __func__);
}


@end
