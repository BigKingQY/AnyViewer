//
//  NSDictionary+BKAdd.m
//  sz
//
//  Created by BigKing on 2020/12/30.
//  Copyright © 2020 hqht. All rights reserved.
//

#import "NSDictionary+BKAdd.h"

@implementation NSDictionary (BKAdd)

+ (NSDictionary *)regularFontSize:(CGFloat)fontSize color:(uint32_t)color {
    return @{NSFontAttributeName:[UIFont systemFontOfSize:fontSize],
             NSForegroundColorAttributeName:[UIColor colorWithRGB:color]
    };
}

+ (NSDictionary *)mediumFontSize:(CGFloat)fontSize color:(uint32_t)color {
    return @{NSFontAttributeName:[UIFont fontWithName:@"PingFangSC-Medium" size:fontSize],
             NSForegroundColorAttributeName:[UIColor colorWithRGB:color]
    };
}

+ (NSDictionary *)boldFontSize:(CGFloat)fontSize color:(uint32_t)color {
    return @{NSFontAttributeName:[UIFont boldSystemFontOfSize:fontSize],
             NSForegroundColorAttributeName:[UIColor colorWithRGB:color]
    };
}

+ (NSDictionary *)textFont:(UIFont *)font color:(uint32_t)color {
    return @{NSFontAttributeName:font,
             NSForegroundColorAttributeName:[UIColor colorWithRGB:color]
    };
}



@end
