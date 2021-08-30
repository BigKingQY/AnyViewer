//
//  BaseCollectionViewController.m
//  JZMerchant
//
//  Created by BigKing on 2021/5/6.
//

#import "BaseCollectionViewController.h"
#import "UIScrollView+BKAdd.h"

@implementation BaseCollectionViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.view addSubview:self.collectionView];
}

//MARK: Delegate && DataSource - 代理

- (NSInteger)numberOfSectionsInCollectionView:(UICollectionView *)collectionView {
    return 1;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
    return 0;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    UICollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"cell" forIndexPath:indexPath];
    
    return cell;
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {}

//MARK: Public Methods

- (void)startRefresh{
    self.pageNum = 1;
    [self.collectionView startRefresh];
}

- (void)endRefreshing{
    [self.collectionView endRefreshing];
}

- (BOOL)isLoadMore {
    return self.pageNum > 1;
}

- (void)resetEnd:(BOOL)isEnd {
    self->_isEnd = isEnd;
}

- (void)addRefreshHeader{
    [self.collectionView addHeaderWithTarget:self action:@selector(refreshData)];
}

- (void)removeHeader{
    [self.collectionView removeHeader];
}

- (void)addMoreFooter{
    [self.collectionView addFooterWithTarget:self action:@selector(loadMoreData)];
}

- (void)removeFooter{
    [self.collectionView removeFooter];
}

- (void)addEmptyDelegator {
    self.emptyDelegator = [BaseEmptyDelegator new];
    self.collectionView.emptyDataSetDelegate = self.emptyDelegator;
    self.collectionView.emptyDataSetSource = self.emptyDelegator;
}

- (void)refreshData {}

- (void)loadMoreData {}

//MARK: Public EmptyDelegator

- (void)emptyDelegatorSubTitle:(NSString *)subTitle {
    [self.emptyDelegator reset];
    self.emptyDelegator.subtitle = [[NSAttributedString alloc] initWithString:subTitle attributes:@{NSFontAttributeName:[UIFont systemFontOfSize:20], NSForegroundColorAttributeName:[UIColor colorWithHexString:@"666666"]}];
}

- (void)emptyDelegatorImage:(NSString *)imageName {
    [self.emptyDelegator reset];
    self.emptyDelegator.image = [UIImage imageNamed:imageName];
}

- (void)emptyDelegatorImage:(NSString *)imageName subTitle:(NSString *)subTitle {
    [self.emptyDelegator reset];
    self.emptyDelegator.image = [UIImage imageNamed:imageName];
    self.emptyDelegator.subtitle = [[NSAttributedString alloc] initWithString:subTitle attributes:@{NSFontAttributeName:[UIFont systemFontOfSize:16], NSForegroundColorAttributeName:[UIColor colorWithHexString:@"666666"]}];
}

- (void)emptyDelegatorOffset:(CGFloat)offset {
    self.emptyDelegator.offset = offset;
}

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    
    if (self.navigationController) {
        self.collectionView.frame = CGRectMake(0, KSTATUS_NAVIGATION_H, self.view.width, self.view.height-KHOME_INDICATOR_H-KSTATUS_NAVIGATION_H);
    }else {
        self.collectionView.frame = CGRectMake(0, KSTATUS_BAR_H, self.view.width, self.view.height-KHOME_INDICATOR_H-KSTATUS_BAR_H);
    }
}

- (UICollectionView *)collectionView {
    if (!_collectionView) {
        
        UICollectionViewFlowLayout *layout = [UICollectionViewFlowLayout new];
        _collectionView = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:layout];
        _collectionView.delegate = self;
        _collectionView.dataSource = self;
        _collectionView.backgroundColor = UIColor.clearColor;
        [_collectionView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:@"cell"];
        if (@available(iOS 11.0, *)) {
            _collectionView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
        }
    }
    return _collectionView;
}

@end
