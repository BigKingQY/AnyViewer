//
//  UIScrollView+BKAdd.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "UIScrollView+BKAdd.h"
@import MJRefresh;

@implementation UIScrollView (BKAdd)

- (void)startRefresh{
    [self.mj_header beginRefreshing];
}

- (void)addHeaderWithTarget:(id)target action:(SEL)action{
    
    MJRefreshNormalHeader *header = [MJRefreshNormalHeader headerWithRefreshingTarget:target refreshingAction:action];
    
//    MJRefreshGifHeader *header = [MJRefreshGifHeader headerWithRefreshingTarget:target refreshingAction:action];
//
//    NSMutableArray<UIImage *> *images = [NSMutableArray array];
//    for (int i = 0; i < 259; i++) {
//        UIImage *image = [UIImage imageNamed:[NSString stringWithFormat:@"%03d", i]];
//        [images addObject:image];
//    }
//    [header setImages:images.copy duration:1 forState:MJRefreshStateRefreshing];
//    [header setImages:@[images.firstObject] forState:MJRefreshStateIdle];
//
//    header.gifView.mj_w = 75;
//    header.gifView.mj_h = 42;
//    [header layoutIfNeeded];
//
//    header.gifView.contentMode = UIViewContentModeScaleAspectFit;

    [header setTitle:@"" forState:MJRefreshStateIdle];
    [header setTitle:@"" forState:MJRefreshStatePulling];
    [header setTitle:@"" forState:MJRefreshStateRefreshing];
    
    header.lastUpdatedTimeLabel.hidden = YES;
    header.stateLabel.hidden = YES;
    
    self.mj_header = header;
}


- (void)addFooterWithTarget:(id)target action:(SEL)action{
    MJRefreshBackNormalFooter *footer = [MJRefreshBackNormalFooter footerWithRefreshingTarget:target refreshingAction:action];
    footer.stateLabel.font = [UIFont systemFontOfSize:12];
    footer.stateLabel.textColor = [UIColor colorWithHexString:@"999999"];
    footer.arrowView.image = nil;
    [footer setTitle:@"- THE END -" forState:MJRefreshStateNoMoreData];
    [footer setTitle:@"拼命加载中..." forState:MJRefreshStateRefreshing];
    [footer setTitle:@"上拉更多精彩~" forState:MJRefreshStateIdle];
    footer.stateLabel.textColor = [UIColor colorWithHexString:@"434651"];
    self.mj_footer = footer;
}

- (void)removeHeader{
    self.mj_header = nil;
}

- (void)removeFooter{
    self.mj_footer = nil;
}

- (void)endRefreshing{
    if (self.mj_header && [self.mj_header isRefreshing]) {
        [self.mj_header endRefreshing];
    }
    
    if (self.mj_footer && [self.mj_footer isRefreshing]) {
        [self.mj_footer endRefreshing];
    }
}

- (void)updateFooterWithEnd:(BOOL)isEnd {
    if (isEnd) {
        [self.mj_footer endRefreshingWithNoMoreData];
    }else {
        [self.mj_footer resetNoMoreData];
    }
}

- (void)updateHeaderWithEnd:(BOOL)isEnd {
    if (isEnd) {
        [self.mj_header endRefreshingCompletionBlock];
    } else {
        [self.mj_footer resetNoMoreData];
    }
}

@end
