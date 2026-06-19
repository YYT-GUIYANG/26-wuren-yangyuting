import torch
from torch import nn
from torchvision import transforms,datasets
from torch.utils.data.dataloader import DataLoader
import torch.optim as optim
import torch.nn.functional as F
from torchinfo import summary
import os
from PIL import Image


def is_valid_image(path):
    try:
        with Image.open(path) as img:
            img.load()
        return True
    except Exception:
        return False


class mixed_net(nn.Module):
    def __init__(self):
        super(mixed_net, self).__init__()
        # /** 这里我是采用了四层卷积的方式，原本想着肯定层数更多效果更好，但是考虑到最终结果是想让准确度
        # 高，那么我们现在只需要先验证一下整个流程是ok的，到时候想要更高的再往上添加
        # 所以最终处理后从输入通道3到输出通道256, 卷积核采取的是3x3, padding=1 保持64x64 **/
        self.conv1 = nn.Conv2d(in_channels=3, out_channels=32, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(in_channels=32, out_channels=64, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(in_channels=64, out_channels=128, kernel_size=3, padding=1)
        self.conv4 = nn.Conv2d(in_channels=128, out_channels=256, kernel_size=3, padding=1)
        # /** 池化层统一就用2x2,特征图宽高各减半**/
        self.pool = nn.MaxPool2d(kernel_size=2, stride=2)
        # /** 全连接层首先需要自己手动计算输入256*4*4=4096，然后不断的压缩，最终输出三个评分去判断它的效果**/
        self.fc1 = nn.Linear(256 * 4 * 4, 512)
        self.fc2 = nn.Linear(512, 128)
        self.fc3 = nn.Linear(128, 3)
    def forward(self, x):
        # /** 卷积→激活→池化→的流程**/
        x = F.relu(self.conv1(x))
        x = self.pool(x)
        x = F.relu(self.conv2(x))
        x = self.pool(x)
        x = F.relu(self.conv3(x))
        x = self.pool(x)
        x = F.relu(self.conv4(x))
        x = self.pool(x)
        # /**在这里就被展成一维向量，然后不断的进行压缩**/
        x = x.view(x.size(0), -1)
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        # /**最终输出3个类别分数，交给 CrossEntropyLoss **/
        x = self.fc3(x)
        return x

if __name__ == "__main__":
    #图像转换
    transform = transforms.Compose(
        [
            transforms.Resize([64, 64]),
            transforms.ToTensor(),
            transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
        ]
    )
    
    #超参数设置
    BATCH_SIZE = 1024
    EPOCH = 200

    #加载数据
    trainset = datasets.ImageFolder(root=r'dataset/train', transform=transform, is_valid_file=is_valid_image)
    testset1 = datasets.ImageFolder(root=r'dataset/test1', transform=transform, is_valid_file=is_valid_image)
    testset2 = datasets.ImageFolder(root=r'dataset/test2', transform=transform, is_valid_file=is_valid_image)

    print(f"训练集图片数量: {len(trainset)}")
    print(f"测试集1图片数量: {len(testset1)}")
    print(f"测试集2图片数量: {len(testset2)}")
    
    train_loader = DataLoader(trainset, batch_size=BATCH_SIZE, shuffle=True, pin_memory=True)
    test_loader1 = DataLoader(testset1, batch_size=BATCH_SIZE, shuffle=True, pin_memory=True)
    test_loader2 = DataLoader(testset2, batch_size=BATCH_SIZE, shuffle=True, pin_memory=True)

    #创建网络
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    net = mixed_net().to(device)
    
    #打印网络信息
    summary(net, input_size=(1, 3, 64, 64), device=device)
    print(f'标签对应的ID: {trainset.class_to_idx}')

    #设置优化器、损失函数
    criterion = nn.CrossEntropyLoss()
    optimizer =optim.SGD(net.parameters(), lr=0.01, momentum=0.9)
    # optimizer = optim.Adam(net.parameters(), lr=0.001, weight_decay=1e-4)

    #开始训练

    print("Start")
    best_acc = 0.0
    half = len(train_loader) // 2

    for epoch in range(EPOCH):
        net.train()
        train_loss = 0.0
        seen = 0

        for batch_id, (datas, labels) in enumerate(train_loader):
            datas, labels = datas.to(device), labels.to(device)

            optimizer.zero_grad()

            outputs = net(datas)

            loss = criterion(outputs, labels)

            loss.backward()

            optimizer.step()

            train_loss += loss.item() * datas.size(0)
            seen += datas.size(0)
            avg_loss = train_loss / seen

            if batch_id == half - 1 or batch_id == len(train_loader) - 1:
                print(f"Train Epoch : {epoch + 1}  Loss : {avg_loss:.6f}")

        net.eval()
        test_loss_sum = 0.0
        correct = 0
        total = 0
        with torch.no_grad():
            for datas, labels in test_loader1:
                datas, labels = datas.to(device), labels.to(device)
                outputs = net(datas)
                test_loss_sum += criterion(outputs, labels).item() * datas.size(0)
                pred = outputs.argmax(dim=1)
                correct += (pred == labels).sum().item()
                total += labels.size(0)

        acc = 100.0 * correct / total
        print(f"Test -- Average Loss : {test_loss_sum / total:.4f}, Accuracy : {acc:.3f}")

        if acc > best_acc:
            best_acc = acc
            os.makedirs("pth", exist_ok=True)
            torch.save(net.state_dict(), "pth/model_best.pth")
