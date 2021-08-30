//
//  BasePageViewController.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/10.
//

#import "BasePageViewController.h"

@interface BasePageViewController ()

@end

@implementation BasePageViewController

//MARK: LifeCycle - 生命周期

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.view addSubview:self.categoryView];
    [self.view addSubview:self.listContainerView];
    
    self.currentIdx = 0;
}


//MARK: - JXCategoryViewDelegate

- (void)categoryView:(JXCategoryBaseView *)categoryView didSelectedItemAtIndex:(NSInteger)index {
    self.currentIdx = index;
    //侧滑手势处理
    self.navigationController.interactivePopGestureRecognizer.enabled = (index == 0);
}

- (void)categoryView:(JXCategoryBaseView *)categoryView didScrollSelectedItemAtIndex:(NSInteger)index {
    self.currentIdx = index;
}

- (void)categoryView:(JXCategoryBaseView *)categoryView didClickSelectedItemAtIndex:(NSInteger)index {
    self.currentIdx = index;
    [self.listContainerView didClickSelectedItemAtIndex:index];
}

//MARK: - JXCategoryListContainerViewDelegate

- (id<JXCategoryListContentViewDelegate>)listContainerView:(JXCategoryListContainerView *)listContainerView initListForIndex:(NSInteger)index {
    return self.controllers[index];
}

- (NSInteger)numberOfListsInlistContainerView:(JXCategoryListContainerView *)listContainerView {
    return self.controllers.count;
}

- (BOOL)listContainerView:(JXCategoryListContainerView *)listContainerView canInitListAtIndex:(NSInteger)index {
    
    return YES;
}

//MARK: Layout - 布局设置

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    self.listContainerView.frame = CGRectMake(0, KSTATUS_NAVIGATION_H, self.view.bounds.size.width, self.view.bounds.size.height-KSTATUS_NAVIGATION_H-KHOME_INDICATOR_H);
}

//MARK: Getter

- (JXCategoryListContainerView *)listContainerView {
    if (_listContainerView == nil) {
        _listContainerView = [[JXCategoryListContainerView alloc] initWithType:JXCategoryListContainerType_ScrollView delegate:self];
    }
    return _listContainerView;
}

- (JXCategoryTitleView *)categoryView {
    if (!_categoryView) {
        _categoryView = [[JXCategoryTitleView alloc] initWithFrame:CGRectMake(0, KSTATUS_NAVIGATION_H, kScreenWidth, 50)];
        _categoryView.delegate = self;
        _categoryView.listContainer = self.listContainerView;
        _categoryView.titleColor = BKColor(0xA2A2A7);
        _categoryView.titleSelectedColor = BKColor(0xE5C352);
        _categoryView.titleFont = [UIFont systemFontOfSize:15];
        _categoryView.cellSpacing = 0;
    }
    return _categoryView;
}


@end
