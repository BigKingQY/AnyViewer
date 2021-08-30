//
//  AMTabBarViewController.m
//  AnyViewer
//
//  Created by BigKing on 2021/8/23.
//

#import "AMTabBarViewController.h"

@interface AMTabBarViewController ()

@end

@implementation AMTabBarViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [self configUI];
}

- (void)configUI {

    self.tabBar.barTintColor  = BKColor(0x0085ff);
    self.tabBar.backgroundImage = [UIImage imageWithColor:BKColor(0x2e3439)];
    UIView *line = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.tabBar.width, 1)];
    line.backgroundColor = BKAColor(0x1E2124, 0.16);
    [self.tabBar addSubview:line];
    
    //视图数组
    NSArray* controllerArr = @[@"AMDeviceViewController",
                               @"AMConnectViewController",
                               @"AMSettingViewController"];
    //标题数组
    NSArray* titleArr = @[NSLocalizedString(@"DEVICE", nil),
                          NSLocalizedString(@"CONNECT", nil),
                          NSLocalizedString(@"SETTING", nil)];
    //图片数组
    NSArray* picArr = @[@"icon_tabBar_device",
                        @"icon_tabBar_connect",
                        @"icon_tabBar_setting"];

    NSMutableArray* array = [[NSMutableArray alloc]init];
    
    for(int i = 0; i < controllerArr.count; i++){
        Class cl=NSClassFromString(controllerArr[i]);
        
        UIViewController *controller = [[cl alloc]init];
        BaseNavigationViewController *nv = [BaseNavigationViewController rootVC:controller];
        controller.title = titleArr[i];
        nv.tabBarItem.image = [[UIImage imageNamed:[NSString stringWithFormat:@"%@_n",picArr[i]]] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal];
        //设置选中时的图片
        nv.tabBarItem.selectedImage = [[UIImage imageNamed:[NSString stringWithFormat:@"%@_s",picArr[i]]] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal];
        [nv.tabBarItem setTitleTextAttributes:@{NSForegroundColorAttributeName:BKColor(0x0085ff)} forState:UIControlStateSelected];
        [nv.tabBarItem setTitleTextAttributes:@{NSForegroundColorAttributeName:BKColor(0x797979)} forState:UIControlStateNormal];
        [array addObject:nv];
    }
    
    self.viewControllers = array;
    self.selectedIndex = 0;
}


@end
